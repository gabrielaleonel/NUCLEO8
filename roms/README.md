# ROMs

Place your CHIP-8 ROM files (`.ch8`, `.c8`, `.rom`, `.bin`) in this directory.

## Test ROMs

For development and testing, you can use ROMs from these sources:

- [Timendus CHIP-8 Test Suite](https://github.com/Timendus/chip8-test-suite) - Comprehensive opcode and quirk tests
- [BC_test.ch8](https://github.com/kripod/chip8-roms/tree/master/games) - Basic test ROM
- [Corax+ test](https://github.com/Timendus/chip8-test-suite) - Opcode conformance test

## ROM Format

CHIP-8 ROMs are raw binary files, typically 2-4 KB in size. They contain no header - code starts executing at address `0x200`.
