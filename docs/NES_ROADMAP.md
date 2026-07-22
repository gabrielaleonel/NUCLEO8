# NES Support Roadmap

This document outlines the planned path from CHIP-8 to NES emulation.

## Hardware Comparison

| Feature | CHIP-8 | NES |
|---------|--------|-----|
| CPU | Custom VM | Ricoh 2A03 (MOS 6502) |
| Clock Speed | ~600 Hz | 1.79 MHz |
| Resolution | 64x32 mono | 256x240 color |
| RAM | 4 KB | 2 KB + 2 KB VRAM |
| Sprites | XOR draw | 64 sprites, 8x8 or 8x16 |
| Sound | 1 buzzer | 5 channels (2 pulse, triangle, noise, DMC) |
| Input | 16 keys | 2 buttons + D-pad per controller |

## Planned Milestones

### Phase 3.1 - 6502 CPU Core
- [ ] Implement 6502 instruction set (official + unofficial opcodes)
- [ ] Memory-mapped I/O architecture ($0000-$FFFF)
- [ ] CPU timing and cycle accuracy
- [ ] Interrupt handling (NMI, IRQ, Reset)
- [ ] Unit tests with established 6502 test ROMs (nestest, etc.)

### Phase 3.2 - PPU (Picture Processing Unit)
- [ ] Background rendering (nametables, attributes, pattern tables)
- [ ] Sprite rendering (OAM, 8 sprites per scanline limit)
- [ ] Scrolling (loopy registers, fine X scroll)
- [ ] Palette and color emphasis
- [ ] PPU timing relative to CPU (3 PPU dots per 1 CPU cycle)

### Phase 3.3 - APU and Input
- [ ] Pulse wave channels (duty cycle, sweep, envelope)
- [ ] Triangle wave channel
- [ ] Noise channel
- [ ] DMC (Delta Modulation Channel)
- [ ] Controller input ($4016, $4017)

### Phase 3.4 - Cartridge and Mappers
- [ ] iNES/NES 2.0 ROM header parsing
- [ ] NROM (mapper 0)
- [ ] MMC1 (mapper 1)
- [ ] UxROM (mapper 2)
- [ ] CNROM (mapper 3)
- [ ] MMC3 (mapper 4)

## Architecture Adaptation

The same hybrid C + C# architecture will be reused:

```
core/
├── chip8/     (existing)
│   ├── include/chip8/
│   └── src/
├── nes/       (new)
│   ├── include/nes/
│   │   ├── nes.h              # Public API (same pattern as chip8.h)
│   │   ├── nes_types.h        # nes_t struct (CPU + PPU + APU + Cart)
│   │   ├── nes_export.h       # Export macros
│   │   ├── cpu_6502.h         # 6502 specific
│   │   └── ppu.h              # PPU specific
│   └── src/
│       ├── nes.c
│       ├── cpu_6502.c
│       ├── ppu.c
│       ├── apu.c
│       ├── mapper.c
│       └── cartridge.c
```

C# bindings will have a `NesNative.cs` with the same pattern of `LibraryImport` calls.
The UI will auto-detect the ROM type and switch backends.
