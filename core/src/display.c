#include "chip8/chip8_types.h"
#include <string.h>

void display_init(uint8_t *framebuffer) {
    memset(framebuffer, 0, CHIP8_FRAMEBUFFER_SIZE);
}

void display_clear(uint8_t *framebuffer) {
    memset(framebuffer, 0, CHIP8_FRAMEBUFFER_SIZE);
}

int display_draw_sprite(uint8_t *framebuffer, const uint8_t *memory,
                        uint16_t i_reg, uint8_t x, uint8_t y, uint8_t n) {
    int collision = 0;
    for (int row = 0; row < n; row++) {
        uint8_t sprite_byte = memory[i_reg + row];
        for (int col = 0; col < 8; col++) {
            if ((sprite_byte & (0x80 >> col)) != 0) {
                int px = (x + col) % CHIP8_SCREEN_WIDTH;
                int py = (y + row) % CHIP8_SCREEN_HEIGHT;
                int index = py * CHIP8_SCREEN_WIDTH + px;
                if (framebuffer[index] == 1) {
                    collision = 1;
                }
                framebuffer[index] ^= 1;
            }
        }
    }
    return collision;
}
