#ifndef CHIP8_H
#define CHIP8_H

#include "definitions.h"

typedef struct chip8 {
    byte V[0xF];
    byte I;
    word PC;
    byte SP;
} chip8;

#endif
