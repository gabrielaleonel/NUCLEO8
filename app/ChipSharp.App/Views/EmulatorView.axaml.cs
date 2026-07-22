using Avalonia;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Media;
using Avalonia.Media.Imaging;
using System;
using System.IO;

namespace ChipSharp.App.Views;

public partial class EmulatorView : UserControl
{
    private WriteableBitmap? _bitmap;
    private readonly int _scale = 8;

    public EmulatorView()
    {
        InitializeComponent();
    }

    public void UpdateFrame(byte[] framebuffer, int width, int height)
    {
        if (_bitmap == null || _bitmap.PixelSize.Width != width || _bitmap.PixelSize.Height != height)
        {
            _bitmap = new WriteableBitmap(
                new PixelSize(width, height),
                new Vector(96, 96),
                Avalonia.Platform.Storage.StoragePixelFormat.Bgra8888);
        }

        using var locked = _bitmap.Lock();
        var span = locked.Span;
        int stride = locked.RowBytes;

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int fbIndex = y * width + x;
                bool pixelOn = framebuffer[fbIndex] != 0;

                int pixelOffset = y * stride + x * 4;
                if (pixelOffset + 3 < span.Length)
                {
                    byte color = pixelOn ? (byte)0xE0 : (byte)0x10;
                    span[pixelOffset + 0] = color;       // B
                    span[pixelOffset + 1] = color;       // G
                    span[pixelOffset + 2] = (byte)(color + 0x20); // R
                    span[pixelOffset + 3] = 255;         // A
                }
            }
        }

        var scaledBitmap = new WriteableBitmap(
            new PixelSize(width * _scale, height * _scale),
            new Vector(96, 96),
            Avalonia.Platform.Storage.StoragePixelFormat.Bgra8888);

        using var srcLocked = _bitmap.Lock();
        using var dstLocked = scaledBitmap.Lock();
        var srcSpan = srcLocked.Span;
        var dstSpan = dstLocked.Span;
        int srcStride = srcLocked.RowBytes;
        int dstStride = dstLocked.RowBytes;

        for (int sy = 0; sy < height; sy++)
        {
            for (int sx = 0; sx < width; sx++)
            {
                int srcOffset = sy * srcStride + sx * 4;
                byte b = srcSpan[srcOffset];
                byte g = srcSpan[srcOffset + 1];
                byte r = srcSpan[srcOffset + 2];
                byte a = srcSpan[srcOffset + 3];

                for (int dy = 0; dy < _scale; dy++)
                {
                    for (int dx = 0; dx < _scale; dx++)
                    {
                        int dstX = sx * _scale + dx;
                        int dstY = sy * _scale + dy;
                        int dstOffset = dstY * dstStride + dstX * 4;
                        if (dstOffset + 3 < dstSpan.Length)
                        {
                            dstSpan[dstOffset + 0] = b;
                            dstSpan[dstOffset + 1] = g;
                            dstSpan[dstOffset + 2] = r;
                            dstSpan[dstOffset + 3] = a;
                        }
                    }
                }
            }
        }

        DisplayCanvas.Children.Clear();
        var image = new Image
        {
            Source = scaledBitmap,
            Width = width * _scale,
            Height = height * _scale
        };
        DisplayCanvas.Children.Add(image);
    }

    public void ClearDisplay()
    {
        DisplayCanvas.Children.Clear();
    }
}
