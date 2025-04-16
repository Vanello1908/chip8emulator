#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "chip8.h"
#include "config.h"
#include "utils.h"

int CLOCKS_PER_FRAME = CLOCKS_PER_SEC / FRAMES_PER_SECOND;

void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    Uint32 * const target_pixel = (Uint32 *) ((Uint8 *) surface->pixels
                                               + y * surface->pitch
                                               + x * surface->format->BytesPerPixel);
    *target_pixel = pixel;
}

void setPixel(SDL_Window* window, int x, int y, int pixel) {

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    for(int i = 0; i < SCREEN_COEFF; i++) {
        for(int j = 0; j < SCREEN_COEFF; j++) {
            set_pixel(surface, x * SCREEN_COEFF + j, y * SCREEN_COEFF + i, pixel);
        }
    }
}

void updateSurface(SDL_Surface* surface, chip8* chip) {
    for(int i = 0; i < SCREEN_Y; i++) {
        for(int j = 0; j < SCREEN_X; j++) {
            if(chip->screen[i][j]) {
                set_pixel(surface, j, i, 0xFFFFFFFF);
            } else {
                set_pixel(surface, j, i, 0x00000000);
            }
        }
    }
}

int processSDLEvents(chip8* chip) {
    SDL_Event windowEvent;
    for(int i = 0; i < 0x10; i++) {
        chip->keysNow[i] = 0;
    }
    while (SDL_PollEvent(&windowEvent))
    {
        if (windowEvent.type == SDL_QUIT)
        {
            return EXIT_SUCCESS;
        }
        if(windowEvent.type == SDL_KEYDOWN) {
            byte key = keyToByte(SDL_GetKeyName(windowEvent.key.keysym.sym));
            if(key < 0x10) {
                chip->keysNow[key] = chip->keys[key] == 0;
                chip->keys[key] = 0x1;
            }
        } else if(windowEvent.type == SDL_KEYUP) {
            byte key = keyToByte(SDL_GetKeyName(windowEvent.key.keysym.sym));
            if(key < 0x10) {
                chip->keys[key] = 0x0;
            }
        }
    }
    return EXIT_FAILURE;
}

int main( int argc, char *argv[] )
{
    const char* romPath = "./roms/5.ch8";
    if(argc > 1) {
        romPath = argv[1];
    }
    chip8* chip = initChip(romPath);

    SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_Window* window = SDL_CreateWindow( "CHIP-8 emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_X * SCREEN_COEFF, SCREEN_Y * SCREEN_COEFF,SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);

    if (window == NULL)
    {
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    int lastClockTime = 0;

    SDL_Surface* screenSurface = SDL_CreateRGBSurface(0, SCREEN_X, SCREEN_Y, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    SDL_Texture* screenTexture = SDL_CreateTextureFromSurface(renderer, screenSurface);
    SDL_Rect textureRect = {0, 0, SCREEN_X, SCREEN_Y};

    while (1) {
        for (int i = 0; i < TICKS_PER_FRAME; i++) {
            if(processSDLEvents(chip) == EXIT_SUCCESS) {
                return EXIT_SUCCESS;
            };
            if(executeInstruction(chip) == ERROR) {
                chip->PC -= 2;
                printf("Address: %04X\nOpcode: %04X\n\n", chip->PC, readWord(chip));
            }
            while (clock() - lastClockTime < CLOCKS_PER_FRAME / TICKS_PER_FRAME) {}
            lastClockTime = clock();
        }
        updateSurface(screenSurface, chip);
        SDL_UpdateTexture(screenTexture, &textureRect, screenSurface->pixels, screenSurface->pitch);

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, screenTexture, NULL, NULL);
        SDL_RenderPresent(renderer);

        updateTimers(chip);
    }
}





