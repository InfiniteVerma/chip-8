#pragma once

#include <iostream>
#include <unordered_map>

#include "SDL3/SDL_keycode.h"
using std::ostream;

#define SDLK_1 0x00000031u /**< '1' */
#define SDLK_2 0x00000032u /**< '2' */
#define SDLK_3 0x00000033u /**< '3' */
#define SDLK_4 0x00000034u /**< '4' */

#define SDLK_Q 0x00000071u /**< 'q' */
#define SDLK_W 0x00000077u /**< 'w' */
#define SDLK_E 0x00000065u /**< 'e' */
#define SDLK_R 0x00000072u /**< 'r' */

#define SDLK_A 0x00000061u /**< 'a' */
#define SDLK_S 0x00000073u /**< 's' */
#define SDLK_D 0x00000064u /**< 'd' */
#define SDLK_F 0x00000066u /**< 'f' */

#define SDLK_Z 0x0000007au /**< 'z' */
#define SDLK_X 0x00000078u /**< 'x' */
#define SDLK_C 0x00000063u /**< 'c' */
#define SDLK_V 0x00000076u /**< 'v' */

class Keypad {
   public:
    enum Pressed {
        KEY_ZERO = 0,
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
        KEY_F,
        NO_KEY_PRESSED,
    };

    enum Action {
        KEY_UP = 0,   // key is released
        KEY_DOWN = 1  // key is pressed
    };

    static Keypad* getInstance() {
        if (!instance) {
            instance = new Keypad();
        }

        return instance;
    }

    static Pressed translateScancodeToChip8(SDL_Keycode);
    static void updateKeyInState(uint8_t key, Action action);
    static void printKeyState();
    static bool isKeyPressed(uint8_t key);

   private:
    Keypad() {}
    static Keypad* instance;
    static const std::unordered_map<int, Pressed> mapping;
    static Action key_state[16];
};

std::ostream& operator<<(std::ostream& os, const Keypad::Pressed& pressed);
