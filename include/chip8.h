#pragma once
#include <constants.h>
#include <result.h>

#include <cstdint>
#include <stack>
#include <string>

class Chip8 {
    uint8_t display_window[WIDTH][HEIGHT];
    uint8_t memory[MEMORY_SIZE]{};
    stack<uint16_t> st;

    uint16_t pc;
    uint16_t index_register;

    uint8_t delay_timer;
    uint8_t sound_timer;

    uint8_t registers[16];

    bool waiting_for_key;
    uint8_t reg_for_key;

    void emulateCycle();

    void clear_screen();
    void jump(uint16_t);
    void jump_with_offset(uint16_t, uint8_t);
    void set_register(uint8_t, uint8_t);
    void add_to_register(uint8_t, uint8_t);
    void binary_coded_dec_conversion(uint8_t);
    void add_to_index_register(uint8_t);
    void set_index_register(uint16_t);
    void set_reg_to_random(uint8_t, uint8_t);
    void display(uint8_t, uint8_t, uint8_t);
    void skip_next_if_equal(uint8_t, uint8_t);
    void skip_next_if_not_equal(uint8_t, uint8_t);
    void skip_if_regs_equal(uint8_t, uint8_t);
    void skip_if_regs_not_equal(uint8_t, uint8_t);
    void skip_if_key_pressed(uint8_t);
    void skip_if_key_not_pressed(uint8_t);
    void shift_right_regs(uint8_t, uint8_t);
    void shift_left_regs(uint8_t, uint8_t);
    void set_font(uint8_t);
    void set_delay_timer(uint8_t);
    void set_sound_timer(uint8_t);
    void get_key(uint8_t);
    void store_to_registers(uint8_t);
    void load_into_registers(uint8_t);
    void push_frame(uint16_t);
    void pop_frame();

    void binary_or(uint8_t, uint8_t);
    void binary_and(uint8_t, uint8_t);
    void binary_xor(uint8_t, uint8_t);
    void add_regs(uint8_t, uint8_t);
    void sub_regs(uint8_t, uint8_t);
    void sub_regs_reverse(uint8_t, uint8_t);

   public:
    Result<int, std::string> readROM(const char* romPath);
    void run();
};
