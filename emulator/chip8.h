#ifndef CHIP8_H
#define CHIP8_H

#include "definitions.h"

typedef struct chip8 {
    byte screen[0x40][0x40];
    byte memory[0x1000];
    byte stack[0x10];
    byte V[0x10];
    word I;
    word DT;
    word ST;
    word PC;
    word SP;
} chip8;

typedef enum chip8result {
    SUCCESS,
    ERROR
} chip8result;

chip8* createChip();
void resetChip(chip8* chip);
void clearDisplay(chip8* chip);
void writeROM(chip8* chip, byte* rom, word size);
byte readByte(chip8* chip);
word readWord(chip8* chip);
chip8result executeInstruction(chip8* chip);
word startChip(chip8* chip);


#endif
