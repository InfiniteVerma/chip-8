#include <keypad.h>

#include <cassert>
#include <iostream>

Keypad* Keypad::instance = nullptr;
Keypad::Action Keypad::key_state[16] = {KEY_UP};

std::ostream& operator<<(std::ostream& os, const Keypad::Pressed& pressed) {
    switch (pressed) {
        case Keypad::KEY_ZERO:
            return os << "KEY_ZERO";
        case Keypad::KEY_ONE:
            return os << "KEY_ONE";
        case Keypad::KEY_TWO:
            return os << "KEY_TWO";
        case Keypad::KEY_THREE:
            return os << "KEY_THREE";
        case Keypad::KEY_FOUR:
            return os << "KEY_FOUR";
        case Keypad::KEY_FIVE:
            return os << "KEY_FIVE";
        case Keypad::KEY_SIX:
            return os << "KEY_SIX";
        case Keypad::KEY_SEVEN:
            return os << "KEY_SEVEN";
        case Keypad::KEY_EIGHT:
            return os << "KEY_EIGHT";
        case Keypad::KEY_NINE:
            return os << "KEY_NINE";
        case Keypad::KEY_A:
            return os << "KEY_A";
        case Keypad::KEY_B:
            return os << "KEY_B";
        case Keypad::KEY_C:
            return os << "KEY_C";
        case Keypad::KEY_D:
            return os << "KEY_D";
        case Keypad::KEY_E:
            return os << "KEY_E";
        case Keypad::KEY_F:
            return os << "KEY_F";
        case Keypad::NO_KEY_PRESSED:
            return os << "NO_KEY_PRESSED";
        default:
            assert(0);
    }
}

const std::unordered_map<int, Keypad::Pressed> Keypad::mapping = {
    {SDLK_1, KEY_ONE},   {SDLK_2, KEY_TWO},   {SDLK_3, KEY_THREE}, {SDLK_4, KEY_C},

    {SDLK_Q, KEY_FOUR},  {SDLK_W, KEY_FIVE},  {SDLK_E, KEY_SIX},   {SDLK_R, KEY_D},

    {SDLK_A, KEY_SEVEN}, {SDLK_S, KEY_EIGHT}, {SDLK_D, KEY_NINE},  {SDLK_F, KEY_E},

    {SDLK_Z, KEY_A},     {SDLK_X, KEY_ZERO},  {SDLK_C, KEY_B},     {SDLK_V, KEY_F},
};

Keypad::Pressed Keypad::translateScancodeToChip8(SDL_Keycode code) {
    Keypad::Pressed ret = NO_KEY_PRESSED;

    if (mapping.find(code) == mapping.end()) return NO_KEY_PRESSED;
    return mapping.at(code);
}

void Keypad::updateKeyInState(uint8_t key, Action action) {
    assert(key <= 0xF);
    assert(action == KEY_UP || action == KEY_DOWN);
    key_state[key] = action;
}

bool Keypad::isKeyPressed(uint8_t key) {
    assert(key <= 0xF);
    return (key_state[key] == KEY_DOWN);
}

void Keypad::printKeyState() {
    for (int i = 0; i < 16; i++) {
        if (key_state[i] == KEY_DOWN) std::cout << std::hex << i << std::dec << " is pressed!\n";
    }
    std::cout << "DONE\n";
}
