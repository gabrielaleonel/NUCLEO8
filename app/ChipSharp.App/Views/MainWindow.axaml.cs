using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Interactivity;
using Avalonia.Platform.Storage;
using ChipSharp.Bindings;
using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;

namespace ChipSharp.App.Views;

public partial class MainWindow : Window
{
    private IntPtr _chip;
    private bool _running;
    private bool _paused;
    private CancellationTokenSource? _cts;

    private const int CYCLES_PER_FRAME = 12;
    private const int FPS = 60;

    public MainWindow()
    {
        InitializeComponent();
        _chip = Chip8Native.chip8_init();
        _running = false;
        _paused = false;

        KeyDown += OnKeyDown;
        KeyUp += OnKeyUp;
        Closed += OnClosed;

        StartEmulationLoop();
    }

    private void StartEmulationLoop()
    {
        _cts = new CancellationTokenSource();
        Task.Run(() => EmulationLoop(_cts.Token));
    }

    private async Task EmulationLoop(CancellationToken token)
    {
        var interval = TimeSpan.FromMilliseconds(1000.0 / FPS);

        while (!token.IsCancellationRequested)
        {
            if (_running && !_paused)
            {
                for (int i = 0; i < CYCLES_PER_FRAME; i++)
                {
                    Chip8Native.chip8_cycle(_chip);
                }
                Chip8Native.chip8_tick_timers(_chip);

                if (Chip8Native.chip8_get_draw_flag(_chip))
                {
                    Chip8Native.chip8_clear_draw_flag(_chip);
                    var fb = Chip8Native.chip8_get_framebuffer(_chip);
                    int w = Chip8Native.chip8_get_display_width();
                    int h = Chip8Native.chip8_get_display_height();

                    byte[] pixels = new byte[w * h];
                    Marshal.Copy(fb, pixels, 0, pixels.Length);

                    Avalonia.Threading.Dispatcher.UIThread.Post(() =>
                    {
                        EmulatorView.UpdateFrame(pixels, w, h);
                        UpdateDebugPanel();
                    });
                }
            }

            await Task.Delay(interval, token).ConfigureAwait(false);
        }
    }

    private void UpdateDebugPanel()
    {
        if (DebugView.IsVisible)
        {
            DebugView.UpdateState(_chip);
        }
    }

    private void OnKeyDown(object? sender, KeyEventArgs e)
    {
        uint key = MapKey(e.Key);
        if (key < 16)
        {
            Chip8Native.chip8_key_down(_chip, (byte)key);
        }
    }

    private void OnKeyUp(object? sender, KeyEventArgs e)
    {
        uint key = MapKey(e.Key);
        if (key < 16)
        {
            Chip8Native.chip8_key_up(_chip, (byte)key);
        }
    }

    private static uint MapKey(Key key)
    {
        return key switch
        {
            Key.D1 => 0x1, Key.D2 => 0x2, Key.D3 => 0x3, Key.D4 => 0xC,
            Key.Q => 0x4, Key.W => 0x5, Key.E => 0x6, Key.R => 0xD,
            Key.A => 0x7, Key.S => 0x8, Key.D => 0x9, Key.F => 0xE,
            Key.Z => 0xA, Key.X => 0x0, Key.C => 0xB, Key.V => 0xF,
            _ => 0xFFFF
        };
    }

    private async void OnOpenRom(object? sender, RoutedEventArgs e)
    {
        var topLevel = GetTopLevel(this);
        if (topLevel == null) return;

        var files = await topLevel.StorageProvider.OpenFilePickerAsync(new FilePickerOpenOptions
        {
            Title = "Open CHIP-8 ROM",
            AllowMultiple = false,
            FileTypeFilter = new[]
            {
                new FilePickerFileType("CHIP-8 ROM") { Patterns = new[] { "*.ch8", "*.c8", "*.rom", "*.bin" } },
                new FilePickerFileType("All Files") { Patterns = new[] { "*.*" } }
            }
        });

        if (files.Count > 0)
        {
            string? path = files[0].TryGetLocalPath();
            if (path != null)
            {
                Chip8Native.chip8_reset(_chip);
                int result = Chip8Native.chip8_load_rom(_chip, path);
                if (result == 0)
                {
                    _running = true;
                    _paused = false;
                    StatusText.Text = $"Running: {Path.GetFileName(path)}";
                }
                else
                {
                    StatusText.Text = $"Error loading ROM: {path}";
                }
            }
        }
    }

    private async void OnSaveState(object? sender, RoutedEventArgs e)
    {
        var topLevel = GetTopLevel(this);
        if (topLevel == null) return;

        var file = await topLevel.StorageProvider.SaveFilePickerAsync(new FilePickerSaveOptions
        {
            Title = "Save State",
            SuggestedFileName = "state.sav"
        });

        if (file != null)
        {
            string? path = file.TryGetLocalPath();
            if (path != null)
            {
                int result = Chip8Native.chip8_save_state(_chip, path);
                StatusText.Text = result == 0 ? "State saved!" : "Error saving state";
            }
        }
    }

    private async void OnLoadState(object? sender, RoutedEventArgs e)
    {
        var topLevel = GetTopLevel(this);
        if (topLevel == null) return;

        var files = await topLevel.StorageProvider.OpenFilePickerAsync(new FilePickerOpenOptions
        {
            Title = "Load State",
            AllowMultiple = false,
            FileTypeFilter = new[]
            {
                new FilePickerFileType("Save State") { Patterns = new[] { "*.sav" } },
                new FilePickerFileType("All Files") { Patterns = new[] { "*.*" } }
            }
        });

        if (files.Count > 0)
        {
            string? path = files[0].TryGetLocalPath();
            if (path != null)
            {
                int result = Chip8Native.chip8_load_state(_chip, path);
                StatusText.Text = result == 0 ? "State loaded!" : "Error loading state";
            }
        }
    }

    private void OnPauseResume(object? sender, RoutedEventArgs e)
    {
        _paused = !_paused;
        StatusText.Text = _paused ? "Paused" : "Running";
    }

    private void OnReset(object? sender, RoutedEventArgs e)
    {
        Chip8Native.chip8_reset(_chip);
        _running = false;
        _paused = false;
        StatusText.Text = "Ready - Open a ROM to start";
        EmulatorView.ClearDisplay();
    }

    private void OnToggleDebug(object? sender, RoutedEventArgs e)
    {
        DebugView.IsVisible = !DebugView.IsVisible;
        StatusText.Text = DebugView.IsVisible ? "Debugger ON" : "Debugger OFF";
    }

    private void OnExit(object? sender, RoutedEventArgs e)
    {
        Close();
    }

    private void OnClosed(object? sender, EventArgs e)
    {
        _cts?.Cancel();
        _cts?.Dispose();
        if (_chip != IntPtr.Zero)
        {
            Chip8Native.chip8_destroy(_chip);
            _chip = IntPtr.Zero;
        }
    }
}
