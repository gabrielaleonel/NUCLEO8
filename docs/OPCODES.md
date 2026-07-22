# CHIP-8 Opcodes Reference

Complete list of all 35 CHIP-8 opcodes implemented in ChipSharp.

## Notation

| Symbol | Meaning |
|--------|---------|
| NNN | Address (12-bit) |
| NN | 8-bit constant |
| N | 4-bit constant |
| X | 4-bit register identifier |
| Y | 4-bit register identifier |
| VF | Flag register (V0F) |

## Opcode Table

| Opcode | Mnemonic | Description |
|--------|----------|-------------|
| `00E0` | CLS | Clear the display |
| `00EE` | RET | Return from subroutine |
| `1NNN` | JP addr | Jump to address NNN |
| `2NNN` | CALL addr | Call subroutine at NNN |
| `3XNN` | SE Vx, byte | Skip next instruction if Vx == NN |
| `4XNN` | SNE Vx, byte | Skip next instruction if Vx != NN |
| `5XY0` | SE Vx, Vy | Skip next instruction if Vx == Vy |
| `6XNN` | LD Vx, byte | Set Vx = NN |
| `7XNN` | ADD Vx, byte | Set Vx = Vx + NN (no carry flag) |
| `8XY0` | LD Vx, Vy | Set Vx = Vy |
| `8XY1` | OR Vx, Vy | Set Vx = Vx OR Vy |
| `8XY2` | AND Vx, Vy | Set Vx = Vx AND Vy |
| `8XY3` | XOR Vx, Vy | Set Vx = Vx XOR Vy |
| `8XY4` | ADD Vx, Vy | Set Vx = Vx + Vy, VF = carry |
| `8XY5` | SUB Vx, Vy | Set Vx = Vx - Vy, VF = NOT borrow |
| `8XY6` | SHR Vx | Set Vx = Vx >> 1, VF = lost bit |
| `8XY7` | SUBN Vx, Vy | Set Vx = Vy - Vx, VF = NOT borrow |
| `8XYE` | SHL Vx | Set Vx = Vx << 1, VF = lost bit |
| `9XY0` | SNE Vx, Vy | Skip next instruction if Vx != Vy |
| `ANNN` | LD I, addr | Set I = NNN |
| `BNNN` | JP V0, addr | Jump to address NNN + V0 |
| `CXNN` | RND Vx, byte | Set Vx = random byte AND NN |
| `DXYN` | DRW Vx, Vy, n | Draw sprite at (Vx, Vy), height N. VF = collision |
| `EX9E` | SKP Vx | Skip next instruction if key Vx is pressed |
| `EXA1` | SKNP Vx | Skip next instruction if key Vx is NOT pressed |
| `FX07` | LD Vx, DT | Set Vx = delay timer value |
| `FX0A` | LD Vx, K | Wait for key press, store in Vx (blocking) |
| `FX15` | LD DT, Vx | Set delay timer = Vx |
| `FX18` | LD ST, Vx | Set sound timer = Vx |
| `FX1E` | ADD I, Vx | Set I = I + Vx |
| `FX29` | LD F, Vx | Set I = location of sprite for hex digit Vx |
| `FX33` | LD B, Vx | Store BCD of Vx at I, I+1, I+2 |
| `FX55` | LD [I], Vx | Store V0..Vx in memory starting at I |
| `FX65` | LD Vx, [I] | Read V0..Vx from memory starting at I |
