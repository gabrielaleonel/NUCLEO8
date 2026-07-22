using Avalonia.Controls;
using ChipSharp.Bindings;
using System;
using System.Text;

namespace ChipSharp.App.Views;

public partial class DebugView : UserControl
{
    public DebugView()
    {
        InitializeComponent();
    }

    public void UpdateState(IntPtr chip)
    {
        var sb = new StringBuilder();

        for (int row = 0; row < 4; row++)
        {
            sb.Clear();
            for (int col = 0; col < 4; col++)
            {
                int idx = row * 4 + col;
                sb.Append($"V{idx:X}: {Chip8Native.chip8_get_register(chip, (byte)idx):X2}  ");
            }
            switch (row)
            {
                case 0: RegisterText.Text = sb.ToString().TrimEnd(); break;
                case 1: RegisterText2.Text = sb.ToString().TrimEnd(); break;
                case 2: RegisterText3.Text = sb.ToString().TrimEnd(); break;
                case 3: RegisterText4.Text = sb.ToString().TrimEnd(); break;
            }
        }

        PcText.Text = $"PC: {Chip8Native.chip8_get_pc(chip):X4}";
        IndexText.Text = $"I:  {Chip8Native.chip8_get_index(chip):X4}";
        SpText.Text = $"SP: {Chip8Native.chip8_get_sp(chip):X2}";
        DelayText.Text = $"Delay: {Chip8Native.chip8_get_delay_timer(chip):X2}";
        SoundText.Text = $"Sound: {Chip8Native.chip8_get_sound_timer(chip):X2}";

        sb.Clear();
        byte sp = Chip8Native.chip8_get_sp(chip);
        for (int i = 0; i < sp && i < 16; i++)
        {
            sb.AppendLine($"[{i}] {Chip8Native.chip8_get_stack(chip, (byte)i):X4}");
        }
        StackText.Text = sb.Length > 0 ? sb.ToString().TrimEnd() : "(empty)";

        sb.Clear();
        ushort pc = Chip8Native.chip8_get_pc(chip);
        for (int i = 0; i < 16; i++)
        {
            sb.Append($"{Chip8Native.chip8_get_memory(chip, (ushort)(pc + i)):X2} ");
        }
        MemoryText.Text = sb.ToString().TrimEnd();
    }
}
