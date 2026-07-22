#include "unity.h"
#include "chip8/chip8.h"
#include <string.h>

static chip8_t *chip;

void setUp(void) {
    chip = chip8_init();
}

void tearDown(void) {
    chip8_destroy(chip);
}

/* ===== Init Tests ===== */

void test_init_pc(void) {
    TEST_ASSERT_EQUAL_UINT16(0x200, chip8_get_pc(chip));
}

void test_init_registers_zero(void) {
    for (int i = 0; i < 16; i++) {
        TEST_ASSERT_EQUAL_UINT8(0, chip8_get_register(chip, (uint8_t)i));
    }
}

void test_init_index_zero(void) {
    TEST_ASSERT_EQUAL_UINT16(0, chip8_get_index(chip));
}

void test_init_timers_zero(void) {
    TEST_ASSERT_EQUAL_UINT8(0, chip8_get_delay_timer(chip));
    TEST_ASSERT_EQUAL_UINT8(0, chip8_get_sound_timer(chip));
}

void test_init_stack_empty(void) {
    TEST_ASSERT_EQUAL_UINT8(0, chip8_get_sp(chip));
}

void test_init_fontset_loaded(void) {
    /* Font for '0' is F0 90 90 90 F0 */
    TEST_ASSERT_EQUAL_UINT8(0xF0, chip8_get_memory(chip, 0x000));
    TEST_ASSERT_EQUAL_UINT8(0x90, chip8_get_memory(chip, 0x001));
    TEST_ASSERT_EQUAL_UINT8(0x90, chip8_get_memory(chip, 0x002));
    TEST_ASSERT_EQUAL_UINT8(0x90, chip8_get_memory(chip, 0x003));
    TEST_ASSERT_EQUAL_UINT8(0xF0, chip8_get_memory(chip, 0x004));

    /* Font for 'A' is F0 90 F0 90 90 */
    TEST_ASSERT_EQUAL_UINT8(0xF0, chip8_get_memory(chip, 0x028));
    TEST_ASSERT_EQUAL_UINT8(0x90, chip8_get_memory(chip, 0x029));
    TEST_ASSERT_EQUAL_UINT8(0xF0, chip8_get_memory(chip, 0x02A));
    TEST_ASSERT_EQUAL_UINT8(0x90, chip8_get_memory(chip, 0x02B));
    TEST_ASSERT_EQUAL_UINT8(0x90, chip8_get_memory(chip, 0x02C));
}

void test_init_running(void) {
    TEST_ASSERT_TRUE(chip8_is_running(chip));
}

/* ===== ROM Loading Tests ===== */

void test_load_rom(void) {
    uint8_t rom[] = { 0x12, 0x00 };
    int result = chip8_load_rom_data(chip, rom, sizeof(rom));
    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_UINT16(0x200, chip8_get_pc(chip));
}

void test_load_rom_overflow(void) {
    uint8_t rom[100];
    memset(rom, 0, sizeof(rom));
    int result = chip8_load_rom_data(chip, rom, sizeof(rom));
    TEST_ASSERT_EQUAL_INT(0, result);
}

/* ===== Opcode: 6XNN - LD Vx, byte ===== */

void test_opcode_6xnn(void) {
    uint8_t rom[] = { 0x60, 0x42 };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip);
    TEST_ASSERT_EQUAL_UINT8(0x42, chip8_get_register(chip, 0));
}

/* ===== Opcode: 7XNN - ADD Vx, byte ===== */

void test_opcode_7xnn(void) {
    uint8_t rom[] = { 0x60, 0x10, 0x70, 0x05 };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip); /* LD V0, 0x10 */
    chip8_cycle(chip); /* ADD V0, 0x05 */
    TEST_ASSERT_EQUAL_UINT8(0x15, chip8_get_register(chip, 0));
}

void test_opcode_7xnn_no_carry(void) {
    uint8_t rom[] = { 0x60, 0xFF, 0x70, 0x02 };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip); /* LD V0, 0xFF */
    chip8_cycle(chip); /* ADD V0, 0x02 -> V0=0x01, VF should NOT be affected */
    TEST_ASSERT_EQUAL_UINT8(0x01, chip8_get_register(chip, 0));
}

/* ===== Opcode: 8XY0 - LD Vx, Vy ===== */

void test_opcode_8xy0(void) {
    uint8_t rom[] = { 0x60, 0x42, 0x61, 0x13, 0x80, 0x10 };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip); /* LD V0, 0x42 */
    chip8_cycle(chip); /* LD V1, 0x13 */
    chip8_cycle(chip); /* LD V0, V1 */
    TEST_ASSERT_EQUAL_UINT8(0x13, chip8_get_register(chip, 0));
}

/* ===== Opcode: 8XY1 - OR Vx, Vy ===== */

void test_opcode_8xy1(void) {
    uint8_t rom[] = { 0x60, 0x0F, 0x61, 0xF0, 0x80, 0x11 };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip); /* LD V0, 0x0F */
    chip8_cycle(chip); /* LD V1, 0xF0 */
    chip8_cycle(chip); /* OR V0, V1 */
    TEST_ASSERT_EQUAL_UINT8(0xFF, chip8_get_register(chip, 0));
}

/* ===== Opcode: 8XY2 - AND Vx, Vy ===== */

void test_opcode_8xy2(void) {
    uint8_t rom[] = { 0x60, 0x0F, 0x61, 0xFF, 0x80, 0x12 };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip); /* LD V0, 0x0F */
    chip8_cycle(chip); /* LD V1, 0xFF */
    chip8_cycle(chip); /* AND V0, V1 */
    TEST_ASSERT_EQUAL_UINT8(0x0F, chip8_get_register(chip, 0));
}

/* ===== Opcode: 8XY3 - XOR Vx, Vy ===== */

void test_opcode_8xy3(void) {
    uint8_t rom[] = { 0x60, 0xFF, 0x61, 0x0F, 0x80, 0x13 };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip); /* LD V0, 0xFF */
    chip8_cycle(chip); /* LD V1, 0x0F */
    chip8_cycle(chip); /* XOR V0, V1 */
    TEST_ASSERT_EQUAL_UINT8(0xF0, chip8_get_register(chip, 0));
}

/* ===== Opcode: 8XY4 - ADD Vx, Vy (carry) ===== */

void test_opcode_8xy4_no_carry(void) {
    uint8_t rom[] = { 0x60, 0x10, 0x61, 0x05, 0x80, 0x14 };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip);
    chip8_cycle(chip);
    chip8_cycle(chip);
    TEST_ASSERT_EQUAL_UINT8(0x15, chip8_get_register(chip, 0));
    TEST_ASSERT_EQUAL_UINT8(0x00, chip8_get_register(chip, 0xF));
}

void test_opcode_8xy4_carry(void) {
    uint8_t rom[] = { 0x60, 0xFF, 0x61, 0x02, 0x80, 0x14 };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip);
    chip8_cycle(chip);
    chip8_cycle(chip);
    TEST_ASSERT_EQUAL_UINT8(0x01, chip8_get_register(chip, 0));
    TEST_ASSERT_EQUAL_UINT8(0x01, chip8_get_register(chip, 0xF));
}

/* ===== Opcode: 8XY5 - SUB Vx, Vy ===== */

void test_opcode_8xy5_no_borrow(void) {
    uint8_t rom[] = { 0x60, 0x20, 0x61, 0x10, 0x80, 0x15 };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip);
    chip8_cycle(chip);
    chip8_cycle(chip);
    TEST_ASSERT_EQUAL_UINT8(0x10, chip8_get_register(chip, 0));
    TEST_ASSERT_EQUAL_UINT8(0x01, chip8_get_register(chip, 0xF));
}

void test_opcode_8xy5_borrow(void) {
    uint8_t rom[] = { 0x60, 0x10, 0x61, 0x20, 0x80, 0x15 };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip);
    chip8_cycle(chip);
    chip8_cycle(chip);
    TEST_ASSERT_EQUAL_UINT8(0xF0, chip8_get_register(chip, 0));
    TEST_ASSERT_EQUAL_UINT8(0x00, chip8_get_register(chip, 0xF));
}

/* ===== Opcode: 8XY6 - SHR Vx ===== */

void test_opcode_8xy6_lsb_1(void) {
    uint8_t rom[] = { 0x60, 0x03, 0x80, 0x16 };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip);
    chip8_cycle(chip);
    TEST_ASSERT_EQUAL_UINT8(0x01, chip8_get_register(chip, 0));
    TEST_ASSERT_EQUAL_UINT8(0x01, chip8_get_register(chip, 0xF));
}

void test_opcode_8xy6_lsb_0(void) {
    uint8_t rom[] = { 0x60, 0x04, 0x80, 0x16 };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip);
    chip8_cycle(chip);
    TEST_ASSERT_EQUAL_UINT8(0x02, chip8_get_register(chip, 0));
    TEST_ASSERT_EQUAL_UINT8(0x00, chip8_get_register(chip, 0xF));
}

/* ===== Opcode: 8XY7 - SUBN Vx, Vy ===== */

void test_opcode_8xy7(void) {
    uint8_t rom[] = { 0x60, 0x10, 0x61, 0x20, 0x80, 0x17 };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip);
    chip8_cycle(chip);
    chip8_cycle(chip);
    TEST_ASSERT_EQUAL_UINT8(0x10, chip8_get_register(chip, 0));
    TEST_ASSERT_EQUAL_UINT8(0x01, chip8_get_register(chip, 0xF));
}

/* ===== Opcode: 8XYE - SHL Vx ===== */

void test_opcode_8xye_msb_1(void) {
    uint8_t rom[] = { 0x60, 0x80, 0x80, 0x1E };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip);
    chip8_cycle(chip);
    TEST_ASSERT_EQUAL_UINT8(0x00, chip8_get_register(chip, 0));
    TEST_ASSERT_EQUAL_UINT8(0x01, chip8_get_register(chip, 0xF));
}

void test_opcode_8xye_msb_0(void) {
    uint8_t rom[] = { 0x60, 0x40, 0x80, 0x1E };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip);
    chip8_cycle(chip);
    TEST_ASSERT_EQUAL_UINT8(0x80, chip8_get_register(chip, 0));
    TEST_ASSERT_EQUAL_UINT8(0x00, chip8_get_register(chip, 0xF));
}

/* ===== Opcode: ANNN - LD I, addr ===== */

void test_opcode_annn(void) {
    uint8_t rom[] = { 0xA1, 0x23 };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip);
    TEST_ASSERT_EQUAL_UINT16(0x123, chip8_get_index(chip));
}

/* ===== Opcode: CXNN - RND Vx, byte ===== */

void test_opcode_cxnn(void) {
    uint8_t rom[] = { 0xC0, 0x00 };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip);
    TEST_ASSERT_EQUAL_UINT8(0x00, chip8_get_register(chip, 0));
}

void test_opcode_cxnn_mask(void) {
    uint8_t rom[] = { 0xC0, 0xFF };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip);
    uint8_t val = chip8_get_register(chip, 0);
    TEST_ASSERT_TRUE(val <= 0xFF);
}

/* ===== Opcode: 2NNN / 00EE - CALL / RET ===== */

void test_opcode_call_ret(void) {
    /* Put a RET instruction at 0x300, then CALL 0x300 */
    uint8_t rom[] = {
        0x23, 0x00,  /* CALL 0x300 */
        0x60, 0x42   /* LD V0, 0x42 (should not execute if RET works) */
    };
    chip8_load_rom_data(chip, rom, sizeof(rom));

    /* Write RET at 0x300 */
    chip8_key_down(chip, 0); /* dummy */

    /* Manually write RET at 0x300 */
    chip8_load_rom_data(chip, (uint8_t[]){0x23, 0x00, 0x60, 0x42, 0x00, 0xEE}, 6);

    /* Actually: put CALL 0x204 at 0x200, and RET at 0x204 */
    uint8_t rom2[] = {
        0x22, 0x04,  /* CALL 0x204 */
        0x60, 0xFF,  /* LD V0, 0xFF */
        0x00, 0xEE   /* RET */
    };
    chip8_load_rom_data(chip, rom2, sizeof(rom2));
    chip8_cycle(chip); /* CALL 0x204 */
    chip8_cycle(chip); /* LD V0, 0xFF */
    chip8_cycle(chip); /* RET -> back to 0x202 */
    chip8_cycle(chip); /* LD V0, 0xFF (same instruction again) */
    TEST_ASSERT_EQUAL_UINT8(0xFF, chip8_get_register(chip, 0));
}

/* ===== Opcode: 3XNN / 4XNN - SE / SNE ===== */

void test_opcode_3xnn_skip(void) {
    uint8_t rom[] = { 0x60, 0x42, 0x30, 0x42, 0x60, 0x00 };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip); /* LD V0, 0x42 */
    chip8_cycle(chip); /* SE V0, 0x42 -> skip next */
    chip8_cycle(chip); /* should be at 0x206 now (skipped 0x204) */
    TEST_ASSERT_EQUAL_UINT8(0x42, chip8_get_register(chip, 0));
}

void test_opcode_4xnn_skip(void) {
    uint8_t rom[] = { 0x60, 0x42, 0x40, 0x42, 0x60, 0x00 };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip); /* LD V0, 0x42 */
    chip8_cycle(chip); /* SNE V0, 0x42 -> skip next */
    chip8_cycle(chip); /* should be at 0x206 now */
    TEST_ASSERT_EQUAL_UINT8(0x42, chip8_get_register(chip, 0));
}

/* ===== Opcode: FX33 - BCD ===== */

void test_opcode_fx33(void) {
    uint8_t rom[] = { 0x60, 0xFF, 0xA2, 0x00, 0xF0, 0x33 };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip); /* LD V0, 255 */
    chip8_cycle(chip); /* LD I, 0x200 (overwrite ROM area, ok for test) */
    chip8_cycle(chip); /* BCD V0 */
    /* I=0x200: 2, I+1=0x201: 5, I+2=0x202: 5 */
    TEST_ASSERT_EQUAL_UINT8(2, chip8_get_memory(chip, 0x200));
    TEST_ASSERT_EQUAL_UINT8(5, chip8_get_memory(chip, 0x201));
    TEST_ASSERT_EQUAL_UINT8(5, chip8_get_memory(chip, 0x202));
}

/* ===== Timer Tests ===== */

void test_delay_timer_tick(void) {
    uint8_t rom[] = { 0x60, 0x05, 0xF0, 0x15 };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip); /* LD V0, 5 */
    chip8_cycle(chip); /* LD DT, V0 */
    TEST_ASSERT_EQUAL_UINT8(5, chip8_get_delay_timer(chip));
    chip8_tick_timers(chip);
    TEST_ASSERT_EQUAL_UINT8(4, chip8_get_delay_timer(chip));
    chip8_tick_timers(chip);
    chip8_tick_timers(chip);
    chip8_tick_timers(chip);
    chip8_tick_timers(chip);
    TEST_ASSERT_EQUAL_UINT8(0, chip8_get_delay_timer(chip));
    chip8_tick_timers(chip);
    TEST_ASSERT_EQUAL_UINT8(0, chip8_get_delay_timer(chip));
}

/* ===== Input Tests ===== */

void test_key_input(void) {
    chip8_key_down(chip, 0x5);
    uint8_t rom[] = { 0xE5, 0x9E };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip); /* SKP V5 */
    /* PC should have skipped: 0x202 -> 0x204 */
    TEST_ASSERT_EQUAL_UINT16(0x204, chip8_get_pc(chip));
}

void test_key_not_pressed(void) {
    uint8_t rom[] = { 0xE5, 0xA1 };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip); /* SKNP V5 -> skip since key 5 is NOT pressed */
    TEST_ASSERT_EQUAL_UINT16(0x204, chip8_get_pc(chip));
}

/* ===== FX0A - Wait for key ===== */

void test_opcode_fx0a_wait(void) {
    uint8_t rom[] = { 0xF0, 0x0A, 0x60, 0x42 };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip); /* LD V0, K -> wait */
    chip8_cycle(chip); /* should still be waiting */
    chip8_cycle(chip); /* still waiting */
    TEST_ASSERT_EQUAL_UINT16(0x200, chip8_get_pc(chip));

    chip8_key_down(chip, 0xA);
    chip8_cycle(chip); /* should detect key and move on */
    chip8_cycle(chip); /* LD V0, 0x42 */
    TEST_ASSERT_EQUAL_UINT8(0x42, chip8_get_register(chip, 0));
}

/* ===== FX29 - Font sprite ===== */

void test_opcode_fx29(void) {
    uint8_t rom[] = { 0x60, 0x00, 0xF0, 0x29 };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip); /* LD V0, 0 */
    chip8_cycle(chip); /* LD F, V0 -> I = 0 */
    TEST_ASSERT_EQUAL_UINT16(0, chip8_get_index(chip));

    uint8_t rom2[] = { 0x60, 0x0A, 0xF0, 0x29 };
    chip8_load_rom_data(chip, rom2, sizeof(rom2));
    chip8_cycle(chip);
    chip8_cycle(chip);
    TEST_ASSERT_EQUAL_UINT16(50, chip8_get_index(chip));
}

/* ===== FX1E - ADD I, Vx ===== */

void test_opcode_fx1e(void) {
    uint8_t rom[] = { 0xA1, 0x00, 0x60, 0x10, 0xF0, 0x1E };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip); /* LD I, 0x100 */
    chip8_cycle(chip); /* LD V0, 0x10 */
    chip8_cycle(chip); /* ADD I, V0 */
    TEST_ASSERT_EQUAL_UINT16(0x110, chip8_get_index(chip));
}

/* ===== BNNN - JP V0, addr ===== */

void test_opcode_bnnn(void) {
    uint8_t rom[] = { 0x60, 0x10, 0xB2, 0x00 };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip); /* LD V0, 0x10 */
    chip8_cycle(chip); /* JP V0, 0x200 -> PC = 0x200 + 0x10 = 0x210 */
    TEST_ASSERT_EQUAL_UINT16(0x210, chip8_get_pc(chip));
}

/* ===== 1NNN - JP addr ===== */

void test_opcode_1nnn(void) {
    uint8_t rom[] = { 0x12, 0x10 };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip);
    TEST_ASSERT_EQUAL_UINT16(0x210, chip8_get_pc(chip));
}

/* ===== 5XY0 - SE Vx, Vy ===== */

void test_opcode_5xy0(void) {
    uint8_t rom[] = { 0x60, 0x42, 0x61, 0x42, 0x50, 0x10, 0x60, 0x00 };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip);
    chip8_cycle(chip);
    chip8_cycle(chip); /* SE V0, V1 -> skip */
    chip8_cycle(chip); /* skipped LD V0, 0x00 */
    TEST_ASSERT_EQUAL_UINT8(0x42, chip8_get_register(chip, 0));
}

/* ===== 9XY0 - SNE Vx, Vy ===== */

void test_opcode_9xy0(void) {
    uint8_t rom[] = { 0x60, 0x42, 0x61, 0x13, 0x90, 0x10, 0x60, 0x00 };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip);
    chip8_cycle(chip);
    chip8_cycle(chip); /* SNE V0, V1 -> skip since 0x42 != 0x13 */
    chip8_cycle(chip); /* skipped */
    TEST_ASSERT_EQUAL_UINT8(0x42, chip8_get_register(chip, 0));
}

/* ===== Serialize / Deserialize ===== */

void test_serialize_roundtrip(void) {
    uint8_t rom[] = { 0x60, 0x42, 0xA1, 0x00 };
    chip8_load_rom_data(chip, rom, sizeof(rom));
    chip8_cycle(chip);
    chip8_cycle(chip);

    size_t sz = chip8_serialize_size();
    uint8_t *buf = (uint8_t *)malloc(sz);
    TEST_ASSERT_NOT_NULL(buf);

    int written = chip8_serialize(chip, buf, sz);
    TEST_ASSERT_EQUAL_INT((int)sz, written);

    chip8_t *chip2 = chip8_init();
    int result = chip8_unserialize(chip2, buf, sz);
    TEST_ASSERT_EQUAL_INT(0, result);

    TEST_ASSERT_EQUAL_UINT16(chip8_get_pc(chip), chip8_get_pc(chip2));
    TEST_ASSERT_EQUAL_UINT8(chip8_get_register(chip, 0), chip8_get_register(chip2, 0));
    TEST_ASSERT_EQUAL_UINT16(chip8_get_index(chip), chip8_get_index(chip2));

    chip8_destroy(chip2);
    free(buf);
}

/* ===== Main ===== */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_init_pc);
    RUN_TEST(test_init_registers_zero);
    RUN_TEST(test_init_index_zero);
    RUN_TEST(test_init_timers_zero);
    RUN_TEST(test_init_stack_empty);
    RUN_TEST(test_init_fontset_loaded);
    RUN_TEST(test_init_running);

    RUN_TEST(test_load_rom);
    RUN_TEST(test_load_rom_overflow);

    RUN_TEST(test_opcode_6xnn);
    RUN_TEST(test_opcode_7xnn);
    RUN_TEST(test_opcode_7xnn_no_carry);
    RUN_TEST(test_opcode_8xy0);
    RUN_TEST(test_opcode_8xy1);
    RUN_TEST(test_opcode_8xy2);
    RUN_TEST(test_opcode_8xy3);
    RUN_TEST(test_opcode_8xy4_no_carry);
    RUN_TEST(test_opcode_8xy4_carry);
    RUN_TEST(test_opcode_8xy5_no_borrow);
    RUN_TEST(test_opcode_8xy5_borrow);
    RUN_TEST(test_opcode_8xy6_lsb_1);
    RUN_TEST(test_opcode_8xy6_lsb_0);
    RUN_TEST(test_opcode_8xy7);
    RUN_TEST(test_opcode_8xye_msb_1);
    RUN_TEST(test_opcode_8xye_msb_0);
    RUN_TEST(test_opcode_annn);
    RUN_TEST(test_opcode_cxnn);
    RUN_TEST(test_opcode_cxnn_mask);
    RUN_TEST(test_opcode_call_ret);
    RUN_TEST(test_opcode_3xnn_skip);
    RUN_TEST(test_opcode_4xnn_skip);
    RUN_TEST(test_opcode_fx33);
    RUN_TEST(test_delay_timer_tick);
    RUN_TEST(test_key_input);
    RUN_TEST(test_key_not_pressed);
    RUN_TEST(test_opcode_fx0a_wait);
    RUN_TEST(test_opcode_fx29);
    RUN_TEST(test_opcode_fx1e);
    RUN_TEST(test_opcode_bnnn);
    RUN_TEST(test_opcode_1nnn);
    RUN_TEST(test_opcode_5xy0);
    RUN_TEST(test_opcode_9xy0);
    RUN_TEST(test_serialize_roundtrip);

    return UNITY_END();
}
