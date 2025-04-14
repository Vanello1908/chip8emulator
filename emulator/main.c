#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "chip8.h"
#include "config.h"

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

void updateDisplay(SDL_Window* window, chip8* chip) {
    for(int i = 0; i < SCREEN_Y; i++) {
        for(int j = 0; j < SCREEN_X; j++) {
            if(chip->screen[i][j] == 1) {
                setPixel(window, j, i, 0xFFFFFF);
            } else {
                setPixel(window, j, i, 0x000000);
            }
        }
    }
}

int main( int argc, char *argv[] )
{
    chip8* chip = initChip("./roms/3.ch8");

    SDL_Init( SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow( "CHIP-8 emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_X * SCREEN_COEFF, SCREEN_Y * SCREEN_COEFF,SDL_WINDOW_ALLOW_HIGHDPI);

    if (window == NULL)
    {
        return 1;
    }

    SDL_Event windowEvent;
    int lastClockTime = 0;

    while (1)
    {
        for (int i = 0; i < TICKS_PER_FRAME; i++) {
            if(executeInstruction(chip) == ERROR) {
                chip->PC -= 2;
                printf("Address: %04X\nOpcode: %04X\n\n", chip->PC, readWord(chip));
            }
        }
        updateDisplay(window, chip);
        SDL_UpdateWindowSurface( window );

        while ( SDL_PollEvent( &windowEvent ) )
        {
            if ( SDL_QUIT == windowEvent.type )
            {
                return EXIT_SUCCESS;
            }
        }

        while (clock() - lastClockTime < CLOCKS_PER_FRAME) {}
        lastClockTime = clock();
    }
}



