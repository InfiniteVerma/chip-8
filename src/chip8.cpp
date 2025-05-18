#include <chip8.h>
#include <display.h>
#include <unistd.h>

#include <cassert>
#include <chrono>
#include <fstream>
#include <random>
#include <thread>

#include "SDL3/SDL_events.h"
#include "constants.h"
#include "iostream"
#include "keypad.h"

// #define DEBUG

#ifdef DEBUG
#define LOG(msg) cout << msg << "\n";
#else
#define LOG(msg)
#endif

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
    LOG("PC: 0x" << hex << pc << dec << " - Instruction: " << hex << instruction << dec);
    //  pc += 2;

    // decode
    uint8_t first_nibble = (instruction >> 12) & 0xF;
    uint8_t second_nibble = (instruction >> 8) & 0xF;
    uint8_t third_nibble = (instruction >> 4) & 0xF;
    uint8_t fourth_nibble = (instruction & 0xF);
    uint8_t second_byte = (instruction & 0xFF);
    uint16_t sec_third_fourth = (instruction & 0xFFF);

    // std::cout << "First nibble: 0x" << std::hex << static_cast<int>(first_nibble) << std::dec
    //           << "\n";
    // std::cout << "Second nibble: 0x" << std::hex << static_cast<int>(second_nibble) << std::dec
    //           << "\n";
    // std::cout << "Third nibble: 0x" << std::hex << static_cast<int>(third_nibble) << std::dec
    //           << "\n";
    // std::cout << "Fourth nibble: 0x" << std::hex << static_cast<int>(fourth_nibble) << std::dec
    //           << "\n";
    // std::cout << "Second byte: 0x" << std::hex << static_cast<int>(second_byte) << std::dec <<
    // "\n"; std::cout << "Second-third-fourth: 0x" << std::hex <<
    // static_cast<int>(sec_third_fourth)
    //           << std::dec << "\n";

    switch (first_nibble) {
        case 0x0:
            if (second_byte == 0xE0) {
                clear_screen();
                pc += 2;
            } else if (second_byte == 0xEE) {
                pop_frame();  // sets pc internally
            } else {
                assert(0);
            }
            break;
        case 0x1:
            jump(sec_third_fourth);  // sets pc internally
            break;
        case 0x2:
            push_frame(sec_third_fourth);  // sets pc internally
            break;
        default:
            pc += 2;  // incrementing program counter here

            switch (static_cast<int>(first_nibble)) {
                case 0x0: {
                    switch (second_byte) {
                        default:
                            cout << "ERROR not supporting instruction: " << hex << instruction
                                 << dec << "\n";
                            assert(0);
                    }
                    break;
                }
                case 0x3:
                    skip_next_if_equal(second_nibble, second_byte);
                    break;
                case 0x4:
                    skip_next_if_not_equal(second_nibble, second_byte);
                    break;
                case 0x5:
                    skip_if_regs_equal(second_nibble, third_nibble);
                    break;
                case 0x6:
                    set_register(second_nibble, second_byte);
                    break;
                case 0x7:
                    add_to_register(second_nibble, second_byte);
                    break;
                case 0x8: {
                    LOG("ARITHMETIC");
                    switch (fourth_nibble) {
                        case 0x0:
                            set_register(second_nibble, registers[third_nibble]);
                            break;
                        case 0x1:
                            binary_or(second_nibble, third_nibble);
                            break;
                        case 0x2:
                            binary_and(second_nibble, third_nibble);
                            break;
                        case 0x3:
                            binary_xor(second_nibble, third_nibble);
                            break;
                        case 0x4:
                            add_regs(second_nibble, third_nibble);
                            break;
                        case 0x5:
                            sub_regs(second_nibble, third_nibble);
                            break;
                        case 0x6:
                            shift_right_regs(second_nibble, third_nibble);
                            break;
                        case 0x7:
                            sub_regs_reverse(second_nibble, third_nibble);
                            break;
                        case 0xe:
                            shift_left_regs(second_nibble, third_nibble);
                            break;
                        default:
                            cout << "ERROR not supporting instruction: " << hex << instruction
                                 << dec << "\n";
                            assert(0);
                    }
                }
                case 0x9:
                    skip_if_regs_not_equal(second_nibble, third_nibble);
                    break;
                case 0xA:
                    set_index_register(sec_third_fourth);
                    break;
                case 0xB:
                    jump_with_offset(sec_third_fourth, second_nibble);
                    break;
                case 0xC:
                    set_reg_to_random(second_nibble, second_byte);
                    break;
                case 0xD:
                    display(second_nibble, third_nibble, static_cast<int>(fourth_nibble));
                    break;
                case 0xE: {
                    switch (second_byte) {
                        case 0x9E:
                            skip_if_key_pressed(second_nibble);
                            break;
                        case 0xA1:
                            skip_if_key_not_pressed(second_nibble);
                            break;
                        default:
                            cout << "ERROR not supporting instruction: " << hex << instruction
                                 << dec << "\n";
                            assert(0);
                    }
                    break;
                }
                case 0xF: {
                    switch (second_byte) {
                        case 0x07:
                            set_register(second_nibble, delay_timer);
                            break;
                        case 0x0A:
                            get_key(second_nibble);
                            break;
                        case 0x15:
                            set_delay_timer(second_nibble);
                            break;
                        case 0x18:
                            set_sound_timer(second_nibble);
                            break;
                        case 0x29:
                            set_font(second_nibble);
                            break;
                        case 0x1E:
                            add_to_index_register(second_nibble);
                            break;
                        case 0x33:
                            binary_coded_dec_conversion(second_nibble);
                            break;
                        case 0x55:
                            store_to_registers(second_nibble);
                            break;
                        case 0x65:
                            load_into_registers(second_nibble);
                            break;
                        default:
                            cout << "ERROR not supporting instruction: " << hex << instruction
                                 << dec << "\n";
                            assert(0);
                    }
                    break;
                }
                default:
                    cout << "FAILED because instruction: <" << hex << instruction << dec
                         << "> not supported!\n";
                    assert(0);
            }
    }

    LOG("Instruction complete!");
}

void Chip8::skip_next_if_equal(uint8_t reg, uint8_t val) {
    LOG("SKIP_NEXT_IF_EQUAL: reg: " << static_cast<int>(reg)
                                    << " val: " << static_cast<int>(registers[reg])
                                    << " == val: " << static_cast<int>(val));
    if (registers[reg] == val) pc += 2;
}

void Chip8::skip_next_if_not_equal(uint8_t reg, uint8_t val) {
    LOG("SKIP_NEXT_IF_NOT_EQUAL: reg: " << static_cast<int>(reg)
                                        << " val: " << static_cast<int>(registers[reg])
                                        << " != val: " << static_cast<int>(val));
    if (registers[reg] != val) pc += 2;
}

void Chip8::skip_if_regs_equal(uint8_t reg1, uint8_t reg2) {
    LOG("SKIP_IF_REGS_EQUAL: reg1: " << static_cast<int>(reg1)
                                     << " and reg2: " << static_cast<int>(reg2));
    LOG("SKIP_IF_REGS_EQUAL: val1: " << static_cast<int>(registers[reg1])
                                     << " and val2: " << static_cast<int>(registers[reg2]));
    if (registers[reg1] == registers[reg2]) pc += 2;
}

void Chip8::skip_if_regs_not_equal(uint8_t reg1, uint8_t reg2) {
    LOG("SKIP_IF_REGS_NOT_EQUAL: reg1: " << static_cast<int>(reg1)
                                         << " and reg2: " << static_cast<int>(reg2));
    LOG("SKIP_IF_REGS_NOT_EQUAL: val1: " << static_cast<int>(registers[reg1])
                                         << " and val2: " << static_cast<int>(registers[reg2]));
    if (registers[reg1] != registers[reg2]) pc += 2;
}

void Chip8::skip_if_key_pressed(uint8_t reg1) {
    LOG("SKIP_IF_KEY_PRESSED: " << hex << static_cast<int>(registers[reg1]) << dec);
    uint8_t key = registers[reg1];
    assert(key <= 0xF);
    if (Keypad::getInstance()->isKeyPressed(key)) {
        LOG("PRESSED! updating pc");
        pc += 2;
    } else {
        LOG("NOT PRESSED, not updating pc");
    }
}

void Chip8::skip_if_key_not_pressed(uint8_t reg1) {
    LOG("SKIP_IF_KEY_NOT_PRESSED: " << hex << static_cast<int>(registers[reg1]) << dec);
    uint8_t key = registers[reg1];
    assert(key <= 0xF);
    if (!Keypad::getInstance()->isKeyPressed(key)) {
        LOG("NOT PRESSED! updating pc");
        pc += 2;
    } else {
        LOG("PRESSED! not updating PC");
    }
}

void Chip8::jump(uint16_t addr) {
    LOG("JUMP to: 0x" << hex << addr << dec);
    assert(addr <= 0xFFF);
    pc = addr;
    LOG("PC now: 0x" << hex << pc << dec);
}

void Chip8::jump_with_offset(uint16_t addr, uint8_t reg1) {
    LOG("JUMP WITH OFFSET: " << hex << addr << dec);
    pc = addr + registers[reg1];
}

void Chip8::set_reg_to_random(uint8_t reg, uint8_t val) {
    LOG("SET_REG_TO_RANDOM: reg: " << reg << " - val: " << hex << val << dec);
    static std::random_device rd;   // Non-deterministic seed
    static std::mt19937 gen(rd());  // Mersenne Twister engine
    static std::uniform_int_distribution<> distrib(0, val);
    set_register(reg, static_cast<uint8_t>(distrib(gen)));
}

void Chip8::clear_screen() {
    LOG("CLEAR SCREEN");
    Display::getInstance()->clearScreen();
}

void Chip8::display(uint8_t x, uint8_t y, uint8_t sprite_height) {
    LOG("DISPLAY: x: " << static_cast<int>(x) << " , y: " << static_cast<int>(y)
                       << " sprite_height: " << static_cast<int>(sprite_height));
    assert(x < 16);
    assert(y < 16);
    uint8_t pos_x = registers[x];
    uint8_t pos_y = registers[y];

    // index_register stores start address of sprite
    const uint8_t* sprite_mem_ptr = &memory[index_register];

    LOG("DISPLAY: pos_x: " << pos_x << ", pos_y: " << pos_y
                           << ", sprite_height: " << static_cast<int>(sprite_height));

    for (int row = 0; row < sprite_height; row++) {
        for (int col = 0; col < 8; col++) {
            int x = (pos_x + col) % WIDTH;
            int y = (pos_y + row) % HEIGHT;

            const SDL_FRect pixel = {(float)x * SCALE, (float)y * SCALE, SCALE, SCALE};

            if ((sprite_mem_ptr[row] >> (7 - col)) & 1) {
                display_window[x][y] ^= 1;
            }
        }
    }

    Display::getInstance()->redraw(pos_x, pos_y, display_window, sprite_height);
}

void Chip8::set_index_register(uint16_t val) {
    LOG("SET_INDEX_REGISTER: 0x" << hex << val << dec);
    assert(val <= 0xFFF);
    index_register = val;
}

void Chip8::set_register(uint8_t reg, uint8_t val) {
    LOG("SET_REGISTER: reg: " << static_cast<int>(reg) << " : val: " << static_cast<int>(val));
    assert(reg < 16);
    registers[reg] = val;
}

void Chip8::set_delay_timer(uint8_t reg) {
    LOG("SET_DELAY_TIMER: reg: " << static_cast<int>(reg));
    delay_timer = registers[reg];
}

void Chip8::set_sound_timer(uint8_t reg) {
    LOG("SET_SOUND_TIMER: reg: " << static_cast<int>(reg));
    sound_timer = registers[reg];
}

void Chip8::set_font(uint8_t reg) {
    LOG("SET_FONT reg: " << static_cast<int>(reg));
    index_register = reg;
}

void Chip8::binary_or(uint8_t reg1, uint8_t reg2) {
    LOG("BINARY_OR: reg1: " << static_cast<int>(reg1) << " : reg2: " << static_cast<int>(reg2));
    LOG("BINARY_OR: val1: " << static_cast<int>(registers[reg1])
                            << " : val2: " << static_cast<int>(registers[reg2]));
    registers[reg1] = (registers[reg1] | registers[reg2]);
    LOG("BINARY_OR final: " << static_cast<int>(registers[reg1]));
}

void Chip8::binary_and(uint8_t reg1, uint8_t reg2) {
    LOG("BINARY_AND: reg1: " << static_cast<int>(reg1) << " : reg2: " << static_cast<int>(reg2));
    registers[reg1] = (registers[reg1] & registers[reg2]);
}

void Chip8::binary_xor(uint8_t reg1, uint8_t reg2) {
    LOG("BINARY_XOR: reg1: " << static_cast<int>(reg1) << " : reg2: " << static_cast<int>(reg2));
    registers[reg1] = (registers[reg1] ^ registers[reg2]);
}

void Chip8::add_regs(uint8_t reg1, uint8_t reg2) {
    LOG("ADD_REGS: reg1: " << static_cast<int>(reg1) << " : reg2: " << static_cast<int>(reg2));
    LOG("ADD_REGS: val1: " << static_cast<int>(registers[reg1])
                           << " : val2: " << static_cast<int>(registers[reg2]));

    uint16_t lhs = static_cast<uint16_t>(registers[reg1]);
    uint16_t rhs = static_cast<uint16_t>(registers[reg2]);
    uint16_t sum = lhs + rhs;

    LOG("Sum: " << sum << ", sum & 0xFF: " << (sum & 0xFF));
    registers[reg1] = (sum & 0xFF);
    LOG("Updated reg[" << static_cast<int>(reg1) << "] = " << static_cast<int>(registers[reg1]));

    set_register(0xF, sum > 0xFF ? 1 : 0);
}

void Chip8::sub_regs(uint8_t reg1, uint8_t reg2) {
    LOG("SUB_REGS: reg1: " << static_cast<int>(reg1) << " : reg2: " << static_cast<int>(reg2));

    uint16_t val1 = registers[reg1];
    uint16_t val2 = registers[reg2];

    LOG("SUB_REGS: val: " << val1 << " - " << val2);

    if (val1 >= val2) {
        registers[reg1] = (val1 - val2) & 0xFF;
        set_register(0xF, 1);
    } else {
        registers[reg1] = (0x100 + val1 - val2) & 0xFF;
        set_register(0xF, 0);
    }
}

void Chip8::sub_regs_reverse(uint8_t reg1, uint8_t reg2) {
    LOG("SUB_REGS_REVERSE: reg1: " << static_cast<int>(reg1)
                                   << " : reg2: " << static_cast<int>(reg2));

    uint16_t val1 = registers[reg1];
    uint16_t val2 = registers[reg2];

    if (val2 >= val1) {
        registers[reg1] = (val2 - val1) & 0xFF;
        set_register(0xF, 1);
    } else {
        registers[reg1] = (0x100 + val2 - val1) & 0xFF;
        set_register(0xF, 0);
    }
}

void Chip8::shift_right_regs(uint8_t reg1, uint8_t reg2) {
    LOG("SHIFT_RIGHT_REGS: reg1: " << static_cast<int>(reg1)
                                   << " : reg2: " << static_cast<int>(reg2));

    set_register(0xF, (registers[reg1] & 0x0F) == 1 ? 1 : 0);
    registers[reg1] = (registers[reg1] >> 1) & 0xFF;
}

void Chip8::shift_left_regs(uint8_t reg1, uint8_t reg2) {
    LOG("SHIFT_LEFT_REGS: reg1: " << static_cast<int>(reg1)
                                  << " : reg2: " << static_cast<int>(reg2));

    set_register(0xF, (registers[reg1] & 0xF0) == 1 ? 1 : 0);
    registers[reg1] = (registers[reg1] << 1) & 0xFF;
}

void Chip8::add_to_register(uint8_t reg, uint8_t val) {
    LOG("ADD_TO_REGISTER: reg: " << static_cast<int>(reg) << " : val: " << static_cast<int>(val));
    assert(reg < 16);
    registers[reg] += val;
}

void Chip8::add_to_index_register(uint8_t reg) {
    int sum = index_register + registers[reg];
    index_register = min(0xFFF, sum);
    if (sum > 0xFFF) set_register(0xF, 1);
}

void Chip8::binary_coded_dec_conversion(uint8_t reg) {
    LOG("BINARY_CODED_DEC_CONVERSION: reg: " << static_cast<int>(reg));
    uint8_t val = registers[reg];

    // if VX contains 156 (or 9C in hexadecimal), it would put the number 1 at the address in I, 5
    // in address I + 1, and 6 in address I + 2.
    memory[index_register] = val / 100;
    memory[index_register + 1] = (val / 10) % 10;
    memory[index_register + 2] = val % 10;
}

void Chip8::get_key(uint8_t reg) {
    LOG("GET_KEY and store to reg: " << static_cast<int>(reg));
    if (waiting_for_key && is_key_saved) {
        // this happens if it's the second time get_key is called. Previous iteration has set the
        // flag to true this time, there was a key pressed and saved in 'key_pressed' uint8_t
        LOG("GET_KEY key: " << pressed_key
                            << " was pressed! Putting it in register and resetting vars");
        assert(pressed_key <= 0xF);
        registers[reg] = pressed_key;
        waiting_for_key = false;
        is_key_saved = false;
    } else {
        waiting_for_key = true;
        is_key_saved = false;
        pc -= 2;
    }
}

void Chip8::store_to_registers(uint8_t reg) {
    cout << "STORE_TO_REGISTERS from 0 till: " << static_cast<int>(reg) << "\n";
    // For FX55, the value of each variable register from V0 to VX inclusive (if X is 0, then only
    // V0) will be stored in successive memory addresses, starting with the one that’s stored in I.
    // V0 will be stored at the address in I, V1 will be stored in I + 1, and so on, until VX is
    // stored in I + X.

    uint16_t index_ptr = index_register;
    for (int i = 0; i <= reg; i++) {
        memory[index_ptr + i] = registers[i];
    }
    cout << "DONE\n";
}

void Chip8::load_into_registers(uint8_t reg) {
    LOG("LOAD_INTO_REGISTERS from 0 till: " << static_cast<int>(reg));

    uint16_t index_ptr = index_register;
    for (int i = 0; i <= reg; i++) {
        registers[i] = memory[index_ptr + i];
    }
}

void Chip8::push_frame(uint16_t mem_loc) {
    LOG("PUSH_FRAME: mem_loc: " << hex << mem_loc << dec);
    assert(mem_loc <= 0xFFF);
    LOG("Pushing pc: 0x" << hex << pc + 2 << dec << " to stack");
    st.push(pc + 2);
    pc = mem_loc;
}

void Chip8::pop_frame() {
    LOG("POP FRAME");
    assert(!st.empty());
    pc = st.top();
    LOG("Updating pc to: " << hex << pc << dec);
    st.pop();
}

void Chip8::run() {
    using clock = std::chrono::high_resolution_clock;
    auto next_cycle = clock::now();
    auto last_timer_update = clock::now();
    bool running = true;

    Display::getInstance()->createWindow();

    // uint16_t addr = START_SPRITE_ADDRESS;

    while (running) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {  // loop till all events are handled
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
                break;
            } else if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
                SDL_Keycode key = event.key.key;

                Keypad::Pressed key_pressed = Keypad::getInstance()->translateScancodeToChip8(key);
                if (key_pressed == Keypad::NO_KEY_PRESSED) break;

                Keypad::Action action =
                    event.type == SDL_EVENT_KEY_UP ? Keypad::KEY_UP : Keypad::KEY_DOWN;

                Keypad::getInstance()->updateKeyInState(static_cast<uint16_t>(key_pressed), action);

                if (event.type == SDL_EVENT_KEY_DOWN && waiting_for_key && !is_key_saved) {
                    is_key_saved = true;
                    pressed_key = key_pressed;
                }
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
        auto now = clock::now();
        if (now - last_timer_update >= std::chrono::milliseconds(16)) {
            if (delay_timer > 0) delay_timer--;
            if (sound_timer > 0) sound_timer--;
            last_timer_update += std::chrono::milliseconds(16);
        }

        next_cycle += CYCLE_DELAY;
        std::this_thread::sleep_until(next_cycle);
    }

    Display::getInstance()->destroyWindow();
}
