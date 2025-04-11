#include "chip8.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>

chip8* createChip() {
    chip8* chip = malloc(sizeof(chip8));
    resetChip(chip);
    return chip;
}

void resetChip(chip8 *chip) {
    memset(chip, 0, sizeof(chip8));
    chip->PC = 0x200;
    chip->SP = 0xFFFF;
}

void clearDisplay(chip8 *chip) {
    memset(chip->screen, 0, sizeof(chip->screen));
}

void writeROM(chip8* chip, byte* rom, word size) {
    memcpy(&chip->memory + chip->PC, rom, size);
}

byte readByte(chip8* chip) {
    byte b = chip->memory[chip->PC];
    chip->PC++;
    return b;
}

word readWord(chip8* chip) {
    byte b1 = readByte(chip);
    byte b2 = readByte(chip);
    return parseWord(b1, b2);
}



chip8result executeInstruction(chip8 *chip) {

}

word startChip(chip8 *chip) {
    while (executeInstruction(chip) == SUCCESS && chip->PC != 0x00) {}
    if(chip->PC != 0x00) {
        return chip->PC;
    }
    return 0;
}

