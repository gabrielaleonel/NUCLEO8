#ifndef CHIP8_H
#define CHIP8_H

#include "chip8_export.h"
#include "chip8_types.h"
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

CHIP8_API chip8_t *chip8_init(void);
CHIP8_API void     chip8_destroy(chip8_t *chip);
CHIP8_API void     chip8_reset(chip8_t *chip);

CHIP8_API int chip8_load_rom(chip8_t *chip, const char *filepath);
CHIP8_API int chip8_load_rom_data(chip8_t *chip, const uint8_t *data, size_t size);

CHIP8_API void chip8_cycle(chip8_t *chip);
CHIP8_API void chip8_tick_timers(chip8_t *chip);

CHIP8_API const uint8_t *chip8_get_framebuffer(chip8_t *chip);
CHIP8_API int            chip8_get_display_width(void);
CHIP8_API int            chip8_get_display_height(void);

CHIP8_API void chip8_key_down(chip8_t *chip, uint8_t key);
CHIP8_API void chip8_key_up(chip8_t *chip, uint8_t key);

CHIP8_API uint16_t chip8_get_pc(chip8_t *chip);
CHIP8_API uint8_t  chip8_get_register(chip8_t *chip, uint8_t index);
CHIP8_API uint16_t chip8_get_index(chip8_t *chip);
CHIP8_API uint8_t  chip8_get_delay_timer(chip8_t *chip);
CHIP8_API uint8_t  chip8_get_sound_timer(chip8_t *chip);
CHIP8_API uint8_t  chip8_get_sp(chip8_t *chip);
CHIP8_API uint16_t chip8_get_stack(chip8_t *chip, uint8_t depth);
CHIP8_API uint8_t  chip8_get_memory(chip8_t *chip, uint16_t addr);
CHIP8_API bool     chip8_is_running(chip8_t *chip);
CHIP8_API bool     chip8_get_draw_flag(chip8_t *chip);
CHIP8_API void     chip8_clear_draw_flag(chip8_t *chip);

CHIP8_API size_t chip8_serialize_size(void);
CHIP8_API int    chip8_serialize(chip8_t *chip, uint8_t *buf, size_t buf_size);
CHIP8_API int    chip8_unserialize(chip8_t *chip, const uint8_t *buf, size_t buf_size);
CHIP8_API int    chip8_save_state(chip8_t *chip, const char *filepath);
CHIP8_API int    chip8_load_state(chip8_t *chip, const char *filepath);

#ifdef __cplusplus
}
#endif

#endif /* CHIP8_H */
