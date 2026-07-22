# ChipSharp Architecture

## Overview

ChipSharp uses a **hybrid architecture** separating the emulation core (C) from the user interface (C#).

```
┌─────────────────────────────────────────────────────────┐
│                    C# Application                        │
│  ┌─────────────┐  ┌──────────────┐  ┌───────────────┐  │
│  │ Avalonia UI  │  │   Debugger   │  │  Save/Load    │  │
│  │  Renderer    │  │   Panel      │  │   States      │  │
│  └──────┬──────┘  └──────┬───────┘  └───────┬───────┘  │
│         │                │                   │           │
│  ┌──────┴────────────────┴───────────────────┴───────┐  │
│  │              Chip8Native.cs (P/Invoke)             │  │
│  └───────────────────────┬───────────────────────────┘  │
└──────────────────────────┼──────────────────────────────┘
                           │
                    ┌──────┴──────┐
                    │  P/Invoke   │
                    │  Boundary   │
                    └──────┬──────┘
                           │
┌──────────────────────────┼──────────────────────────────┐
│  ┌───────────────────────┴───────────────────────────┐  │
│  │              chip8.h (C API)                       │  │
│  └───────────────────────┬───────────────────────────┘  │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌───────────┐  │
│  │   CPU    │ │ Display  │ │ Timers   │ │   Input   │  │
│  │ Opcodes  │ │ Framebuf │ │ 60Hz     │ │ 16 keys   │  │
│  └──────────┘ └──────────┘ └──────────┘ └───────────┘  │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐                │
│  │  Memory  │ │ Fontset  │ │  Stack   │                │
│  │  4KB RAM │ │ 0-F hex  │ │ 16 deep  │                │
│  └──────────┘ └──────────┘ └──────────┘                │
│                    chip8.dll / chip8.so                 │
└─────────────────────────────────────────────────────────┘
```

## Data Flow

```
ROM File ──> chip8_load_rom() ──> Memory[0x200..]
                                         │
                              ┌───────────┘
                              v
                     chip8_cycle() x N
                     ┌─────────────────┐
                     │ Fetch Opcode    │
                     │ Decode          │
                     │ Execute         │
                     │ Update PC       │
                     └────────┬────────┘
                              │
                   chip8_tick_timers() x 1
                   ┌──────────────────┐
                   │ Delay timer -= 1 │
                   │ Sound timer -= 1 │
                   └────────┬─────────┘
                            │
                   chip8_get_framebuffer()
                   ┌──────────────────┐
                   │ uint8_t[2048]    │
                   │ (64x32 pixels)   │
                   └────────┬─────────┘
                            │
                     P/Invoke Marshal.Copy
                            │
                   ┌────────┴─────────┐
                   │ WriteableBitmap  │
                   │ Scale 8x         │
                   │ Render to screen │
                   └──────────────────┘
```

## Extension Points

For future NES support (Phase 3):

- `nes.h` will define a `nes_t` struct (CPU 6502 + PPU + APU + Memory)
- Same P/Invoke pattern: `nes_init()`, `nes_cycle()`, `nes_get_framebuffer()`
- C# shell will detect ROM type and switch between chip8/nes backends
- The emulator loop structure (cycle N times + tick timers + render) remains identical
