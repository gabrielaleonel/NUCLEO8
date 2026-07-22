#include "chip8/chip8_types.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern void fontset_load(uint8_t *memory);
extern void display_init(uint8_t *framebuffer);
extern void display_clear(uint8_t *framebuffer);
extern int  display_draw_sprite(uint8_t *framebuffer, const uint8_t *memory,
                                 uint16_t i_reg, uint8_t x, uint8_t y, uint8_t n);
extern void timers_init(chip8_t *chip);
extern void input_init(bool *keys);

static uint16_t fetch_opcode(const chip8_t *chip) {
    uint8_t high = chip->memory[chip->pc];
    uint8_t low  = chip->memory[chip->pc + 1];
    return (uint16_t)((high << 8) | low);
}

static void push_stack(chip8_t *chip, uint16_t value) {
    if (chip->sp < CHIP8_STACK_SIZE) {
        chip->stack[chip->sp] = value;
        chip->sp++;
    }
}

static uint16_t pop_stack(chip8_t *chip) {
    if (chip->sp > 0) {
        chip->sp--;
        return chip->stack[chip->sp];
    }
    return 0;
}

void cpu_init(chip8_t *chip) {
    chip->pc = CHIP8_PROGRAM_START;
    chip->i  = 0;
    chip->sp = 0;
    memset(chip->v, 0, CHIP8_REGISTER_COUNT);
    memset(chip->stack, 0, sizeof(uint16_t) * CHIP8_STACK_SIZE);
    fontset_load(chip->memory);
    display_init(chip->framebuffer);
    timers_init(chip);
    input_init(chip->keys);
    chip->draw_flag = false;
    chip->running = true;
    chip->wait_for_key = false;
}

void cpu_cycle(chip8_t *chip) {
    if (!chip->running || chip->wait_for_key) return;

    uint16_t opcode = fetch_opcode(chip);
    chip->pc += 2;

    uint16_t nnn = opcode & 0x0FFF;
    uint8_t  nn  = opcode & 0x00FF;
    uint8_t  n   = opcode & 0x000F;
    uint8_t  x   = (opcode >> 8) & 0x000F;
    uint8_t  y   = (opcode >> 4) & 0x000F;

    switch (opcode & 0xF000) {
        case 0x0000:
            switch (nn) {
                case 0xE0:
                    display_clear(chip->framebuffer);
                    chip->draw_flag = true;
                    break;
                case 0xEE:
                    chip->pc = pop_stack(chip);
                    break;
                default:
                    /* 0NNN - SYS addr (ignored on modern interpreters) */
                    break;
            }
            break;

        case 0x1000:
            chip->pc = nnn;
            break;

        case 0x2000:
            push_stack(chip, chip->pc);
            chip->pc = nnn;
            break;

        case 0x3000:
            if (chip->v[x] == nn) chip->pc += 2;
            break;

        case 0x4000:
            if (chip->v[x] != nn) chip->pc += 2;
            break;

        case 0x5000:
            if (chip->v[x] == chip->v[y]) chip->pc += 2;
            break;

        case 0x6000:
            chip->v[x] = nn;
            break;

        case 0x7000:
            chip->v[x] += nn;
            break;

        case 0x8000:
            switch (n) {
                case 0x0:
                    chip->v[x] = chip->v[y];
                    break;
                case 0x1:
                    chip->v[x] |= chip->v[y];
                    break;
                case 0x2:
                    chip->v[x] &= chip->v[y];
                    break;
                case 0x3:
                    chip->v[x] ^= chip->v[y];
                    break;
                case 0x4: {
                    uint16_t sum = chip->v[x] + chip->v[y];
                    chip->v[0xF] = (sum > 0xFF) ? 1 : 0;
                    chip->v[x] = (uint8_t)(sum & 0xFF);
                    break;
                }
                case 0x5:
                    chip->v[0xF] = (chip->v[x] >= chip->v[y]) ? 1 : 0;
                    chip->v[x] -= chip->v[y];
                    break;
                case 0x6:
                    chip->v[0xF] = chip->v[x] & 0x1;
                    chip->v[x] >>= 1;
                    break;
                case 0x7:
                    chip->v[0xF] = (chip->v[y] >= chip->v[x]) ? 1 : 0;
                    chip->v[x] = chip->v[y] - chip->v[x];
                    break;
                case 0xE:
                    chip->v[0xF] = (chip->v[x] >> 7) & 0x1;
                    chip->v[x] <<= 1;
                    break;
                default:
                    break;
            }
            break;

        case 0x9000:
            if (chip->v[x] != chip->v[y]) chip->pc += 2;
            break;

        case 0xA000:
            chip->i = nnn;
            break;

        case 0xB000:
            chip->pc = nnn + chip->v[0];
            break;

        case 0xC000:
            chip->v[x] = (uint8_t)(rand() % 256) & nn;
            break;

        case 0xD000: {
            uint8_t vx_val = chip->v[x] % CHIP8_SCREEN_WIDTH;
            uint8_t vy_val = chip->v[y] % CHIP8_SCREEN_HEIGHT;
            chip->v[0xF] = display_draw_sprite(
                chip->framebuffer, chip->memory,
                chip->i, vx_val, vy_val, n) ? 1 : 0;
            chip->draw_flag = true;
            break;
        }

        case 0xE000:
            switch (nn) {
                case 0x9E:
                    if (chip->keys[chip->v[x]]) chip->pc += 2;
                    break;
                case 0xA1:
                    if (!chip->keys[chip->v[x]]) chip->pc += 2;
                    break;
                default:
                    break;
            }
            break;

        case 0xF000:
            switch (nn) {
                case 0x07:
                    chip->v[x] = chip->delay_timer;
                    break;
                case 0x0A:
                    chip->wait_for_key = true;
                    chip->wait_key_register = x;
                    break;
                case 0x15:
                    chip->delay_timer = chip->v[x];
                    break;
                case 0x18:
                    chip->sound_timer = chip->v[x];
                    break;
                case 0x1E:
                    chip->i += chip->v[x];
                    break;
                case 0x29:
                    chip->i = chip->v[x] * 5;
                    break;
                case 0x33: {
                    uint8_t val = chip->v[x];
                    chip->memory[chip->i]     = val / 100;
                    chip->memory[chip->i + 1] = (val / 10) % 10;
                    chip->memory[chip->i + 2] = val % 10;
                    break;
                }
                case 0x55:
                    for (int j = 0; j <= x; j++) {
                        chip->memory[chip->i + j] = chip->v[j];
                    }
                    chip->i += x + 1;
                    break;
                case 0x65:
                    for (int j = 0; j <= x; j++) {
                        chip->v[j] = chip->memory[chip->i + j];
                    }
                    chip->i += x + 1;
                    break;
                default:
                    break;
            }
            break;

        default:
            break;
    }
}

void cpu_handle_key_wait(chip8_t *chip) {
    if (!chip->wait_for_key) return;
    for (int k = 0; k < CHIP8_KEY_COUNT; k++) {
        if (chip->keys[k]) {
            chip->v[chip->wait_key_register] = (uint8_t)k;
            chip->wait_for_key = false;
            return;
        }
    }
}
