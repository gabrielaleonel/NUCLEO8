#include "chip8/chip8.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern void cpu_init(chip8_t *chip);
extern void cpu_cycle(chip8_t *chip);
extern void cpu_handle_key_wait(chip8_t *chip);
extern void timers_tick(chip8_t *chip);

chip8_t *chip8_init(void) {
    chip8_t *chip = (chip8_t *)calloc(1, sizeof(chip8_t));
    if (chip) {
        cpu_init(chip);
    }
    return chip;
}

void chip8_destroy(chip8_t *chip) {
    if (chip) {
        free(chip);
    }
}

void chip8_reset(chip8_t *chip) {
    if (chip) {
        cpu_init(chip);
    }
}

int chip8_load_rom(chip8_t *chip, const char *filepath) {
    if (!chip || !filepath) return -1;

    FILE *f = fopen(filepath, "rb");
    if (!f) return -1;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (size <= 0 || size > CHIP8_MEMORY_SIZE - CHIP8_PROGRAM_START) {
        fclose(f);
        return -1;
    }

    uint8_t *buffer = (uint8_t *)malloc((size_t)size);
    if (!buffer) {
        fclose(f);
        return -1;
    }

    size_t read = fread(buffer, 1, (size_t)size, f);
    fclose(f);

    if ((long)read != size) {
        free(buffer);
        return -1;
    }

    int result = chip8_load_rom_data(chip, buffer, (size_t)size);
    free(buffer);
    return result;
}

int chip8_load_rom_data(chip8_t *chip, const uint8_t *data, size_t size) {
    if (!chip || !data) return -1;
    if (size > CHIP8_MEMORY_SIZE - CHIP8_PROGRAM_START) return -1;

    memcpy(&chip->memory[CHIP8_PROGRAM_START], data, size);
    chip->pc = CHIP8_PROGRAM_START;
    return 0;
}

void chip8_cycle(chip8_t *chip) {
    if (!chip) return;
    if (chip->wait_for_key) {
        cpu_handle_key_wait(chip);
        if (!chip->wait_for_key) {
            chip->pc += 2;
        }
        return;
    }
    cpu_cycle(chip);
}

void chip8_tick_timers(chip8_t *chip) {
    if (chip) timers_tick(chip);
}

const uint8_t *chip8_get_framebuffer(chip8_t *chip) {
    return chip ? chip->framebuffer : NULL;
}

int chip8_get_display_width(void) {
    return CHIP8_SCREEN_WIDTH;
}

int chip8_get_display_height(void) {
    return CHIP8_SCREEN_HEIGHT;
}

void chip8_key_down(chip8_t *chip, uint8_t key) {
    if (chip && key < CHIP8_KEY_COUNT) {
        chip->keys[key] = true;
    }
}

void chip8_key_up(chip8_t *chip, uint8_t key) {
    if (chip && key < CHIP8_KEY_COUNT) {
        chip->keys[key] = false;
    }
}

uint16_t chip8_get_pc(chip8_t *chip) {
    return chip ? chip->pc : 0;
}

uint8_t chip8_get_register(chip8_t *chip, uint8_t index) {
    if (!chip || index >= CHIP8_REGISTER_COUNT) return 0;
    return chip->v[index];
}

uint16_t chip8_get_index(chip8_t *chip) {
    return chip ? chip->i : 0;
}

uint8_t chip8_get_delay_timer(chip8_t *chip) {
    return chip ? chip->delay_timer : 0;
}

uint8_t chip8_get_sound_timer(chip8_t *chip) {
    return chip ? chip->sound_timer : 0;
}

uint8_t chip8_get_sp(chip8_t *chip) {
    return chip ? chip->sp : 0;
}

uint16_t chip8_get_stack(chip8_t *chip, uint8_t depth) {
    if (!chip || depth >= CHIP8_STACK_SIZE) return 0;
    return chip->stack[depth];
}

uint8_t chip8_get_memory(chip8_t *chip, uint16_t addr) {
    if (!chip || addr >= CHIP8_MEMORY_SIZE) return 0;
    return chip->memory[addr];
}

bool chip8_is_running(chip8_t *chip) {
    return chip ? chip->running : false;
}

bool chip8_get_draw_flag(chip8_t *chip) {
    return chip ? chip->draw_flag : false;
}

void chip8_clear_draw_flag(chip8_t *chip) {
    if (chip) chip->draw_flag = false;
}

size_t chip8_serialize_size(void) {
    return sizeof(chip8_t);
}

int chip8_serialize(chip8_t *chip, uint8_t *buf, size_t buf_size) {
    if (!chip || !buf) return -1;
    size_t sz = chip8_serialize_size();
    if (buf_size < sz) return -1;
    memcpy(buf, chip, sz);
    return (int)sz;
}

int chip8_unserialize(chip8_t *chip, const uint8_t *buf, size_t buf_size) {
    if (!chip || !buf) return -1;
    size_t sz = chip8_serialize_size();
    if (buf_size < sz) return -1;
    memcpy(chip, buf, sz);
    return 0;
}

int chip8_save_state(chip8_t *chip, const char *filepath) {
    if (!chip || !filepath) return -1;

    FILE *f = fopen(filepath, "wb");
    if (!f) return -1;

    size_t sz = chip8_serialize_size();
    size_t written = fwrite(chip, 1, sz, f);
    fclose(f);

    return (written == sz) ? 0 : -1;
}

int chip8_load_state(chip8_t *chip, const char *filepath) {
    if (!chip || !filepath) return -1;

    FILE *f = fopen(filepath, "rb");
    if (!f) return -1;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    size_t sz = chip8_serialize_size();
    if ((size_t)size < sz) {
        fclose(f);
        return -1;
    }

    size_t read = fread(chip, 1, sz, f);
    fclose(f);

    return (read == sz) ? 0 : -1;
}
