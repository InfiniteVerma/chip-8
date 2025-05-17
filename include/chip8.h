#pragma once
#include <constants.h>
#include <result.h>

#include <cstdint>
#include <string>

class Chip8 {
    uint8_t display_window[WIDTH][HEIGHT];
    uint8_t memory[MEMORY_SIZE]{};
    uint16_t pc;
    uint16_t index_register;

    uint8_t delay_timer;
    uint8_t sount_timer;

    uint8_t registers[16];

    void emulateCycle();

    void clear_screen();
    void jump(uint16_t);
    void set_register(uint8_t, uint8_t);
    void add_to_register(uint8_t, uint8_t);
    void set_index_register(uint16_t);
    void display(uint8_t, uint8_t, uint8_t);

   public:
    Result<int, std::string> readROM(const char* romPath);
    void run();
};
