using System.Runtime.InteropServices;

namespace ChipSharp.Bindings;

public static partial class Chip8Native
{
    private const string LibName = "chip8";

    [LibraryImport(LibName)]
    public static partial IntPtr chip8_init();

    [LibraryImport(LibName)]
    public static partial void chip8_destroy(IntPtr chip);

    [LibraryImport(LibName)]
    public static partial void chip8_reset(IntPtr chip);

    [LibraryImport(LibName)]
    public static partial int chip8_load_rom(IntPtr chip,
        [MarshalAs(UnmanagedType.LPUTF8Str)] string filepath);

    [LibraryImport(LibName)]
    public static partial int chip8_load_rom_data(IntPtr chip, IntPtr data, nuint size);

    [LibraryImport(LibName)]
    public static partial void chip8_cycle(IntPtr chip);

    [LibraryImport(LibName)]
    public static partial void chip8_tick_timers(IntPtr chip);

    [LibraryImport(LibName)]
    public static partial IntPtr chip8_get_framebuffer(IntPtr chip);

    [LibraryImport(LibName)]
    public static partial int chip8_get_display_width();

    [LibraryImport(LibName)]
    public static partial int chip8_get_display_height();

    [LibraryImport(LibName)]
    public static partial void chip8_key_down(IntPtr chip, byte key);

    [LibraryImport(LibName)]
    public static partial void chip8_key_up(IntPtr chip, byte key);

    [LibraryImport(LibName)]
    public static partial ushort chip8_get_pc(IntPtr chip);

    [LibraryImport(LibName)]
    public static partial byte chip8_get_register(IntPtr chip, byte index);

    [LibraryImport(LibName)]
    public static partial ushort chip8_get_index(IntPtr chip);

    [LibraryImport(LibName)]
    public static partial byte chip8_get_delay_timer(IntPtr chip);

    [LibraryImport(LibName)]
    public static partial byte chip8_get_sound_timer(IntPtr chip);

    [LibraryImport(LibName)]
    public static partial byte chip8_get_sp(IntPtr chip);

    [LibraryImport(LibName)]
    public static partial ushort chip8_get_stack(IntPtr chip, byte depth);

    [LibraryImport(LibName)]
    public static partial byte chip8_get_memory(IntPtr chip, ushort addr);

    [LibraryImport(LibName)]
    [return: MarshalAs(UnmanagedType.U1)]
    public static partial bool chip8_is_running(IntPtr chip);

    [LibraryImport(LibName)]
    [return: MarshalAs(UnmanagedType.U1)]
    public static partial bool chip8_get_draw_flag(IntPtr chip);

    [LibraryImport(LibName)]
    public static partial void chip8_clear_draw_flag(IntPtr chip);

    [LibraryImport(LibName)]
    public static partial nuint chip8_serialize_size();

    [LibraryImport(LibName)]
    public static partial int chip8_serialize(IntPtr chip, IntPtr buf, nuint bufSize);

    [LibraryImport(LibName)]
    public static partial int chip8_unserialize(IntPtr chip, IntPtr buf, nuint bufSize);

    [LibraryImport(LibName)]
    public static partial int chip8_save_state(IntPtr chip,
        [MarshalAs(UnmanagedType.LPUTF8Str)] string filepath);

    [LibraryImport(LibName)]
    public static partial int chip8_load_state(IntPtr chip,
        [MarshalAs(UnmanagedType.LPUTF8Str)] string filepath);
}
