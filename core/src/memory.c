#include "chip8/chip8_types.h"
#include <string.h>
#include <stdio.h>

void memory_init(uint8_t *memory) {
    memset(memory, 0, CHIP8_MEMORY_SIZE);
}

int memory_load_rom(uint8_t *memory, const uint8_t *data, size_t size) {
    if (size > CHIP8_MEMORY_SIZE - CHIP8_PROGRAM_START) {
        return -1;
    }
    memcpy(&memory[CHIP8_PROGRAM_START], data, size);
    return 0;
}

uint8_t memory_read(const uint8_t *memory, uint16_t addr) {
    if (addr >= CHIP8_MEMORY_SIZE) return 0;
    return memory[addr];
}

void memory_write(uint8_t *memory, uint16_t addr, uint8_t value) {
    if (addr < CHIP8_MEMORY_SIZE) {
        memory[addr] = value;
    }
}
