#ifndef CHIP8_H
#define CHIP8_H

#include "definitions.h"

typedef struct chip8 {
    byte memory[0xFFF];
    byte stack[0xF];
    byte V[0xF];
    byte I;
    byte DT;
    byte ST;
    word PC;
    word SP;
} chip8;

#endif
