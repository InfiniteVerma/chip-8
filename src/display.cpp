#include <display.h>
#include <unistd.h>

#include "constants.h"

Display* Display::instance = nullptr;
SDL_Window* Display::window = nullptr;
SDL_Renderer* Display::renderer = nullptr;

void Display::createWindow() {
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("CHIP-8 Emulator", WIDTH, HEIGHT, SDL_WINDOW_OPENGL);

    if (window == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
        return;
    }

    renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == nullptr) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        return;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void Display::destroyWindow() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Display::clearScreen() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void Display::redraw(int pos_x, int pos_y, const uint8_t display_window[WIDTH][HEIGHT],
                     uint8_t sprite_height) {
    for (int row = 0; row < sprite_height; row++) {
        for (int col = 0; col < 8; col++) {
            int x = (pos_x + col) % WIDTH;
            int y = (pos_y + row) % HEIGHT;

            const SDL_FRect pixel = {(float)x * SCALE, (float)y * SCALE, SCALE, SCALE};

            if (display_window[x][y]) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderFillRect(renderer, &pixel);
            } else {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderFillRect(renderer, &pixel);
            }
        }
    }

    SDL_RenderPresent(renderer);
}
