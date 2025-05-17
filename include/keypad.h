#pragma once

#include "SDL3/SDL_keycode.h"
class Keypad {
   public:
    enum Pressed {
        NO_KEY_PRESSED,
        KEY_ONE,
        KEY_TWO,
        KEY_THREE,
        KEY_FOUR,
        KEY_FIVE,
        KEY_SIX,
        KEY_SEVEN,
        KEY_EIGHT,
        KEY_NINE,
        KEY_A,
        KEY_B,
        KEY_C,
        KEY_D,
        KEY_E,
        KEY_F
    };

    static Keypad* getInstance() {
        if (!instance) {
            instance = new Keypad();
        }

        return instance;
    }

    static Pressed translateScancodeToChip8(SDL_Keycode);

   private:
    Keypad() {}
    static Keypad* instance;
};
