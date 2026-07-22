using Avalonia;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.Media;
using Avalonia.Media.Imaging;
using Avalonia.Platform;
using System;
using System.IO;

namespace ChipSharp.App.Views;

public unsafe partial class EmulatorView : UserControl
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
                new Vector(96, 96));
        }

        using (var locked = _bitmap.Lock())
        {
            byte* ptr = (byte*)locked.Address;
            int stride = locked.RowBytes;

            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    int fbIndex = y * width + x;
                    bool pixelOn = framebuffer[fbIndex] != 0;

                    int pixelOffset = y * stride + x * 4;
                    byte color = pixelOn ? (byte)0xE0 : (byte)0x10;
                    ptr[pixelOffset + 0] = color;
                    ptr[pixelOffset + 1] = color;
                    ptr[pixelOffset + 2] = (byte)(color + 0x20);
                    ptr[pixelOffset + 3] = 255;
                }
            }
        }

        var scaledBitmap = new WriteableBitmap(
            new PixelSize(width * _scale, height * _scale),
            new Vector(96, 96));

        using (var srcLocked = _bitmap.Lock())
        using (var dstLocked = scaledBitmap.Lock())
        {
            byte* srcPtr = (byte*)srcLocked.Address;
            byte* dstPtr = (byte*)dstLocked.Address;
            int srcStride = srcLocked.RowBytes;
            int dstStride = dstLocked.RowBytes;

            for (int sy = 0; sy < height; sy++)
            {
                for (int sx = 0; sx < width; sx++)
                {
                    int srcOffset = sy * srcStride + sx * 4;
                    byte b = srcPtr[srcOffset];
                    byte g = srcPtr[srcOffset + 1];
                    byte r = srcPtr[srcOffset + 2];
                    byte a = srcPtr[srcOffset + 3];

                    for (int dy = 0; dy < _scale; dy++)
                    {
                        for (int dx = 0; dx < _scale; dx++)
                        {
                            int dstX = sx * _scale + dx;
                            int dstY = sy * _scale + dy;
                            int dstOffset = dstY * dstStride + dstX * 4;
                            dstPtr[dstOffset + 0] = b;
                            dstPtr[dstOffset + 1] = g;
                            dstPtr[dstOffset + 2] = r;
                            dstPtr[dstOffset + 3] = a;
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
