#ifndef CHIP8_TYPES_H
#define CHIP8_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define CHIP8_MEMORY_SIZE    4096
#define CHIP8_REGISTER_COUNT 16
#define CHIP8_STACK_SIZE     16
#define CHIP8_KEY_COUNT      16
#define CHIP8_SCREEN_WIDTH   64
#define CHIP8_SCREEN_HEIGHT  32
#define CHIP8_FRAMEBUFFER_SIZE (CHIP8_SCREEN_WIDTH * CHIP8_SCREEN_HEIGHT)
#define CHIP8_FONTSET_SIZE   80
#define CHIP8_PROGRAM_START  0x200

typedef struct chip8 {
    uint8_t  memory[CHIP8_MEMORY_SIZE];
    uint8_t  v[CHIP8_REGISTER_COUNT];
    uint16_t i;
    uint16_t pc;
    uint16_t stack[CHIP8_STACK_SIZE];
    uint8_t  sp;
    uint8_t  delay_timer;
    uint8_t  sound_timer;
    uint8_t  framebuffer[CHIP8_FRAMEBUFFER_SIZE];
    bool     keys[CHIP8_KEY_COUNT];
    bool     draw_flag;
    bool     running;
    bool     wait_for_key;
    uint8_t  wait_key_register;
} chip8_t;

typedef void (*chip8_opcode_handler)(chip8_t *chip, uint16_t opcode);

#endif /* CHIP8_TYPES_H */
