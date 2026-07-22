#include "chip8/chip8_types.h"

void timers_init(chip8_t *chip) {
    chip->delay_timer = 0;
    chip->sound_timer = 0;
}

void timers_tick(chip8_t *chip) {
    if (chip->delay_timer > 0) {
        chip->delay_timer--;
    }
    if (chip->sound_timer > 0) {
        chip->sound_timer--;
    }
}

uint8_t timers_get_delay(const chip8_t *chip) {
    return chip->delay_timer;
}

uint8_t timers_get_sound(const chip8_t *chip) {
    return chip->sound_timer;
}

void timers_set_delay(chip8_t *chip, uint8_t value) {
    chip->delay_timer = value;
}

void timers_set_sound(chip8_t *chip, uint8_t value) {
    chip->sound_timer = value;
}
