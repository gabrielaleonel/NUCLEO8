using System.Runtime.InteropServices;
using ChipSharp.Bindings;
using NUnit.Framework;

namespace ChipSharp.Tests;

[TestFixture]
public class Chip8BindingsTests
{
    private IntPtr _chip;

    [SetUp]
    public void Setup()
    {
        _chip = Chip8Native.chip8_init();
    }

    [TearDown]
    public void TearDown()
    {
        if (_chip != IntPtr.Zero)
        {
            Chip8Native.chip8_destroy(_chip);
        }
    }

    [Test]
    public void Init_ReturnsValidPointer()
    {
        Assert.That(_chip, Is.Not.EqualTo(IntPtr.Zero));
    }

    [Test]
    public void Init_PC_At_0x200()
    {
        Assert.That(Chip8Native.chip8_get_pc(_chip), Is.EqualTo(0x200));
    }

    [Test]
    public void Init_Registers_Zero()
    {
        for (int i = 0; i < 16; i++)
        {
            Assert.That(Chip8Native.chip8_get_register(_chip, (byte)i), Is.EqualTo(0));
        }
    }

    [Test]
    public void Init_Index_Zero()
    {
        Assert.That(Chip8Native.chip8_get_index(_chip), Is.EqualTo(0));
    }

    [Test]
    public void Init_Timers_Zero()
    {
        Assert.That(Chip8Native.chip8_get_delay_timer(_chip), Is.EqualTo(0));
        Assert.That(Chip8Native.chip8_get_sound_timer(_chip), Is.EqualTo(0));
    }

    [Test]
    public void Display_Dimensions()
    {
        Assert.That(Chip8Native.chip8_get_display_width(), Is.EqualTo(64));
        Assert.That(Chip8Native.chip8_get_display_height(), Is.EqualTo(32));
    }

    [Test]
    public void Opcode_6XNN_SetRegister()
    {
        byte[] rom = [0x60, 0x42];
        var handle = GCHandle.Alloc(rom, GCHandleType.Pinned);
        try
        {
            Chip8Native.chip8_load_rom_data(_chip, handle.AddrOfPinnedObject(), (nuint)rom.Length);
            Chip8Native.chip8_cycle(_chip);
            Assert.That(Chip8Native.chip8_get_register(_chip, 0), Is.EqualTo(0x42));
        }
        finally
        {
            handle.Free();
        }
    }

    [Test]
    public void Opcode_ANNN_SetIndex()
    {
        byte[] rom = [0xA1, 0x23];
        var handle = GCHandle.Alloc(rom, GCHandleType.Pinned);
        try
        {
            Chip8Native.chip8_load_rom_data(_chip, handle.AddrOfPinnedObject(), (nuint)rom.Length);
            Chip8Native.chip8_cycle(_chip);
            Assert.That(Chip8Native.chip8_get_index(_chip), Is.EqualTo(0x123));
        }
        finally
        {
            handle.Free();
        }
    }

    [Test]
    public void Serialize_Size_GreaterThanZero()
    {
        Assert.That(Chip8Native.chip8_serialize_size(), Is.GreaterThan((nuint)0));
    }

    [Test]
    public void Key_Input()
    {
        Chip8Native.chip8_key_down(_chip, 0x5);
        byte[] rom = [0x65, 0x05, 0xE5, 0x9E];
        var handle = GCHandle.Alloc(rom, GCHandleType.Pinned);
        try
        {
            Chip8Native.chip8_load_rom_data(_chip, handle.AddrOfPinnedObject(), (nuint)rom.Length);
            Chip8Native.chip8_cycle(_chip);
            Chip8Native.chip8_cycle(_chip);
            Assert.That(Chip8Native.chip8_get_pc(_chip), Is.EqualTo(0x206));
        }
        finally
        {
            handle.Free();
        }
    }

    [Test]
    public void Timer_Decrement()
    {
        byte[] rom = [0x60, 0x03, 0xF0, 0x15];
        var handle = GCHandle.Alloc(rom, GCHandleType.Pinned);
        try
        {
            Chip8Native.chip8_load_rom_data(_chip, handle.AddrOfPinnedObject(), (nuint)rom.Length);
            Chip8Native.chip8_cycle(_chip); // LD V0, 3
            Chip8Native.chip8_cycle(_chip); // LD DT, V0
            Assert.That(Chip8Native.chip8_get_delay_timer(_chip), Is.EqualTo(3));
            Chip8Native.chip8_tick_timers(_chip);
            Assert.That(Chip8Native.chip8_get_delay_timer(_chip), Is.EqualTo(2));
        }
        finally
        {
            handle.Free();
        }
    }
}
