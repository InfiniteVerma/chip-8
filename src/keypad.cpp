#include <keypad.h>

Keypad* Keypad::instance = nullptr;
const std::unordered_map<int, Keypad::Pressed> Keypad::mapping = {
    {SDLK_1, KEY_ONE},   {SDLK_2, KEY_ONE},   {SDLK_3, KEY_ONE},  {SDLK_4, KEY_C},

    {SDLK_Q, KEY_FOUR},  {SDLK_W, KEY_FIVE},  {SDLK_E, KEY_SIX},  {SDLK_R, KEY_D},

    {SDLK_A, KEY_SEVEN}, {SDLK_S, KEY_EIGHT}, {SDLK_D, KEY_NINE}, {SDLK_F, KEY_E},

    {SDLK_Z, KEY_A},     {SDLK_X, KEY_ZERO},  {SDLK_C, KEY_B},    {SDLK_V, KEY_F},
};

Keypad::Pressed Keypad::translateScancodeToChip8(SDL_Keycode code) {
    Keypad::Pressed ret = NO_KEY_PRESSED;

    if (mapping.find(code) == mapping.end()) return NO_KEY_PRESSED;
    return mapping.at(code);
}
