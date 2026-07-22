#include "chip8/chip8_types.h"
#include <string.h>

void input_init(bool *keys) {
    memset(keys, 0, sizeof(bool) * CHIP8_KEY_COUNT);
}

void input_key_down(bool *keys, uint8_t key) {
    if (key < CHIP8_KEY_COUNT) {
        keys[key] = true;
    }
}

void input_key_up(bool *keys, uint8_t key) {
    if (key < CHIP8_KEY_COUNT) {
        keys[key] = false;
    }
}

bool input_is_key_pressed(const bool *keys, uint8_t key) {
    if (key >= CHIP8_KEY_COUNT) return false;
    return keys[key];
}

int input_get_pressed_key(const bool *keys) {
    for (int i = 0; i < CHIP8_KEY_COUNT; i++) {
        if (keys[i]) {
            return i;
        }
    }
    return -1;
}
