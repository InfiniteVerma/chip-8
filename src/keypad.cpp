#include <keypad.h>

Keypad* Keypad::instance = nullptr;

Keypad::Pressed Keypad::translateScancodeToChip8(SDL_Keycode code) {
    Keypad::Pressed ret = NO_KEY_PRESSED;
    return ret;
}
