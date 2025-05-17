#include <chip8.h>
#include <display.h>
#include <unistd.h>

#include <cassert>
#include <chrono>
#include <fstream>
#include <thread>

#include "constants.h"
#include "iostream"

Result<int, string> Chip8::readROM(const char* romPath) {
    ifstream rom(romPath, ios::binary | ios::ate);

    if (!rom) {
        return string("ERROR in opening file\n");
    }

    streamsize size = rom.tellg();
    if (size > (MEMORY_SIZE - START_ADDRESS)) {
        return string("ROM too large to fit in memory\n");
    }

    rom.seekg(0, ios::beg);

    if (!rom.read(reinterpret_cast<char*>(&memory[START_ADDRESS]), size)) {
        return string("Failed to read ROM file into memory\n");
    }

    memcpy(&memory[START_SPRITE_ADDRESS], &chip8_fontset, sizeof(chip8_fontset));

    pc = START_ADDRESS;

    for (int i = 0; i < 16; i++) set_register(i, 0);

    memset(display_window, 0, sizeof(display_window));

    return size;
}

void Chip8::emulateCycle() {
    // fetch
    uint16_t instruction = (memory[pc] << 8) | memory[pc + 1];
    cout << "Instruction: " << hex << instruction << dec << "\n";
    pc += 2;

    // decode
    uint8_t first_nibble = (instruction >> 12) & 0xF;
    uint8_t second_nibble = (instruction >> 8) & 0xF;
    uint8_t third_nibble = (instruction >> 4) & 0xF;
    uint8_t fourth_nibble = (instruction & 0xF);
    uint8_t second_byte = (instruction & 0xFF);
    uint16_t sec_third_fourth = (instruction & 0xFFF);

    std::cout << "First nibble: 0x" << std::hex << static_cast<int>(first_nibble) << std::dec
              << "\n";
    std::cout << "Second nibble: 0x" << std::hex << static_cast<int>(second_nibble) << std::dec
              << "\n";
    std::cout << "Third nibble: 0x" << std::hex << static_cast<int>(third_nibble) << std::dec
              << "\n";
    std::cout << "Fourth nibble: 0x" << std::hex << static_cast<int>(fourth_nibble) << std::dec
              << "\n";
    std::cout << "Second byte: 0x" << std::hex << static_cast<int>(second_byte) << std::dec << "\n";
    std::cout << "Second-third-fourth: 0x" << std::hex << static_cast<int>(sec_third_fourth)
              << std::dec << "\n";

    if (instruction == 0xE0) {
        clear_screen();
    } else {
        switch (first_nibble) {
            case 0x1:
                jump(sec_third_fourth);
                break;
            case 0x6:
                set_register(second_nibble, second_byte);
                break;
            case 0x7:
                add_to_register(second_nibble, second_byte);
                break;
            case 0xA:
                set_index_register(sec_third_fourth);
                break;
            case 0xD:
                display(second_nibble, third_nibble, static_cast<int>(fourth_nibble));
                break;
            default:
                cout << "ERROR cmd not found!\n";
        }
    }

    cout << "Instruction complete!\n";
}

void Chip8::jump(uint16_t addr) {
    cout << "JUMP to: " << hex << addr << dec << "\n";
    assert(addr <= 0xFFF);
    pc = addr;
}

void Chip8::clear_screen() {
    cout << "CLEAR SCREEN \n";
    Display::getInstance()->clearScreen();
}

void Chip8::display(uint8_t x, uint8_t y, uint8_t sprite_height) {
    cout << "DISPLAY: x: " << static_cast<int>(x) << " , y: " << static_cast<int>(y)
         << " sprite_height: " << static_cast<int>(sprite_height) << "\n";
    assert(x < 16);
    assert(y < 16);
    int pos_x = registers[x];
    int pos_y = registers[y];

    // index_register stores start address of sprite
    const uint8_t* sprite_mem_ptr = &memory[index_register];

    cout << "DISPLAY: pos_x: " << pos_x << ", pos_y: " << pos_y
         << ", sprite_height: " << static_cast<int>(sprite_height) << "\n";

    for (int row = 0; row < sprite_height; row++) {
        for (int col = 0; col < 8; col++) {
            int x = (pos_x + col) % WIDTH;
            int y = (pos_y + row) % HEIGHT;

            const SDL_FRect pixel = {(float)x * SCALE, (float)y * SCALE, SCALE, SCALE};

            display_window[x][y] = 0;

            if ((sprite_mem_ptr[row] >> (7 - col)) & 1) {
                display_window[x][y] ^= 1;
            }
        }
    }

    Display::getInstance()->redraw(pos_x, pos_y, display_window, sprite_height);
}

void Chip8::set_index_register(uint16_t val) {
    cout << "SET_INDEX_REGISTER: 0x" << hex << val << dec << "\n";
    assert(val <= 0xFFF);
    index_register = val;
}

void Chip8::set_register(uint8_t reg, uint8_t val) {
    cout << "SET_REGISTER: reg: " << static_cast<int>(reg) << " : val: " << static_cast<int>(val)
         << "\n";
    assert(reg < 16);
    registers[reg] = val;
}

void Chip8::add_to_register(uint8_t reg, uint8_t val) {
    cout << "ADD_TO_REGISTER: reg: " << static_cast<int>(reg) << " : val: " << static_cast<int>(val)
         << "\n";
    assert(reg < 16);
    registers[reg] = min(0xFF, registers[reg] + val);
}

void Chip8::run() {
    using clock = std::chrono::high_resolution_clock;
    auto next_cycle = clock::now();
    bool running = true;

    Display::getInstance()->createWindow();

    // uint16_t addr = START_SPRITE_ADDRESS;

    while (running) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
                break;
            }
        }

        if (!running) break;

        emulateCycle();
#if 0
        cout << "Drawing a number\n";
        // TEST
        set_index_register(addr);
        registers[0] = 0;
        registers[1] = 1;
        display(0, 1, 5);

        if(addr == (START_SPRITE_ADDRESS + 80)) addr = START_SPRITE_ADDRESS;
        else addr += 5;

        set_index_register(addr);
        registers[0] = 5;
        registers[1] = 1;
        display(0, 1, 5);
#endif
        next_cycle += CYCLE_DELAY;
        std::this_thread::sleep_until(next_cycle);
    }

    Display::getInstance()->destroyWindow();
}
