#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <constants.h>

class Display {
   public:
    static Display* getInstance() {
        if (instance == nullptr) instance = new Display();
        return instance;
    }

    void clearScreen();
    void createWindow();
    void destroyWindow();
    void redraw(int pos_x, int pos_y, const uint8_t display_window[WIDTH][HEIGHT],
                uint8_t sprite_height);

   private:
    Display() {}
    static Display* instance;
    static SDL_Window* window;
    static SDL_Renderer* renderer;
};
