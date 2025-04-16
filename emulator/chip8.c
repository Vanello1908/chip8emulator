#include "chip8.h"

#include <stdio.h>

#include "utils.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>

chip8* createChip() {
    chip8* chip = malloc(sizeof(chip8));
    resetChip(chip);
    return chip;
}

void resetChip(chip8 *chip) {
    memset(chip, 0, sizeof(chip8));
    memcpy(&chip->memory, &INTERPRETER_DIGITS_STUB, 80);
    chip->PC = 0x200;
    chip->SP = 0;
}

void clearDisplay(chip8 *chip) {
    memset(chip->screen, 0, sizeof(chip->screen));
}

void writeROM(chip8* chip, byte* rom, word size) {
    memcpy((byte*)(chip->memory) + chip->PC, rom, size);
}

byte readByte(chip8* chip) {
    byte b = chip->memory[chip->PC];
    chip->PC = (chip->PC + 1) & 0x0FFF;
    return b;
}

word readWord(chip8* chip) {
    byte b1 = readByte(chip);
    byte b2 = readByte(chip);
    return parseWord(b1, b2);
}

chip8result executeInstruction(chip8 *chip) {
    word instruction = readWord(chip);
    byte n = getNibble(instruction);
    byte nn = getByte(instruction);
    word nnn = getAddress(instruction);
    byte x = getX(instruction);
    byte y = getY(instruction);

    byte buffer = 0;

    if (LOGGING) {
        printf("Address: %03X\nInstruction: %04X\nRegisters: ", chip->PC - 2, instruction);
        for(int i = 0; i < 0x10; i ++) {
            printf("%02X ", chip->V[i]);
        }
        printf("\n\n\n");
    }

    switch(instruction >> 12) {
        case 0x0:
            if(instruction == 0x00E0) { //CLS
                clearDisplay(chip);
            } else if(instruction == 0x00EE) { //RET
                chip->PC = chip->stack[chip->SP];
                chip->SP = (chip->SP - 1) & 0xF;
            }
            break;

        case 0x1: //JP addr
            chip->PC = nnn;
            break;

        case 0x2: //CALL addr
            chip->SP = (chip->SP + 1) & 0xF;
            chip->stack[chip->SP] = chip->PC;
            chip->PC = nnn;
            break;

        case 0x3: //SE Vx, byte
            if(chip->V[x] == nn) {
                chip->PC += 2;
            }
            break;

        case 0x4: //SNE Vx, byte
            if(chip->V[x] != nn) {
                chip->PC += 2;
            }
            break;

        case 0x5: //SE Vx, Vy
            if(n == 0 && chip->V[x] == chip->V[y]) {
                chip->PC += 2;
            }
            break;

        case 0x6: //LD Vx, byte
            chip->V[x] = nn;
            break;

        case 0x7: //ADD Vx, byte
            chip->V[x] += nn;
            break;

        case 0x8:
            switch(instruction & 0xF) {
                case 0x0: //LD Vx, Vy
                    chip->V[x] = chip->V[y];
                    break;
                case 0x1: //OR Vx, Vy
                    chip->V[x] |= chip->V[y];
                    if (VF_RESET) {
                        chip->V[0xF] = 0;
                    }
                    break;
                case 0x2: //AND Vx, Vy
                    chip->V[x] &= chip->V[y];
                    if (VF_RESET) {
                        chip->V[0xF] = 0;
                    }
                    break;
                case 0x3: //XOR Vx, Vy
                    chip->V[x] ^= chip->V[y];
                    if (VF_RESET) {
                        chip->V[0xF] = 0;
                    }
                    break;
                case 0x4: //ADD Vx, Vy
                    buffer = chip->V[x];
                    chip->V[x] += chip->V[y];
                    chip->V[0xF] = chip->V[x] < buffer;
                    break;
                case 0x5: //SUB Vx, Vy
                    buffer = chip->V[x] >= chip->V[y];
                    chip->V[x] = chip->V[x] - chip->V[y];
                    chip->V[0xF] = buffer;
                    break;
                case 0x6: //SHR Vx
                    if(SHIFTING) {
                        chip->V[x] = chip->V[y];
                    }
                    buffer = chip->V[x] & 0b1;
                    chip->V[x] >>= 1;
                    chip->V[0xF] = buffer;
                    break;
                case 0x7: //SUBN Vx, Vy
                    buffer = chip->V[y] >= chip->V[x];
                    chip->V[x] = chip->V[y] - chip->V[x];
                    chip->V[0xF] = buffer;
                    break;
                case 0xE: //SHL Vx
                    if(SHIFTING) {
                        chip->V[x] = chip->V[y];
                    }
                    buffer = (chip->V[x] >> 7) & 0b1;
                    chip->V[x] <<= 1;
                    chip->V[0xF] = buffer;
            }
            break;
        case 0x9: //SNE Vx, Vy
            if(n == 0) {
                if(chip->V[x] != chip->V[y]) {
                    chip->PC += 2;
                }
            }
            break;
        case 0xA: //LD I, addr
            chip->I = nnn;
            break;
        case 0xB: //JP V0, addr
            if(JUMPING) {
                nnn += chip->V[x];
            } else {
                nnn += chip->V[0x0];
            }
            chip->PC = nnn;
            break;
        case 0xC: //RND Vx, byte
            chip->V[x] = (rand() % nn) & nn;
            break;
        case 0xD: //DRW Vx, Vy, nibble
            draw(chip, chip->V[x], chip->V[y], n);
            break;
        case 0xE:
            switch (nn) {
                case 0x9E: //SKP Vx
                    if(chip->keys[chip->V[x] & 0xF] == 0x1) {
                        chip->PC += 2;
                    }
                    break;
                case 0xA1: //SKNP Vx
                    if(chip->keys[chip->V[x] & 0xF] == 0x0) {
                        chip->PC += 2;
                    }
                    break;
            }
            break;
        case 0xF:
            switch (nn) {
                case 0x07: //LD Vx, DT
                    chip->V[x] = chip->DT;
                    break;
                case 0x0A: //LD Vx, K
                    for(byte i = 0; i < 0x10; i++) {
                        if(chip->keysNow[i] == 1) {
                            chip->V[x] = i;
                            buffer = 1;
                        }
                    }
                    if (buffer == 0) {
                        chip->PC -= 2;
                    }
                    break;
                case 0x15: //LD DT, Vx
                    chip->DT = chip->V[x];
                    break;
                case 0x18: //LD ST, Vx
                    chip->ST = chip->V[x];
                    break;
                case 0x1E: //ADD I, Vx
                    writeI(chip, chip->I + chip->V[x]);
                    break;
                case 0x29: //LD F, Vx
                    writeI(chip, (chip->V[x] & 0xF) * 0x5);
                    break;
                case 0x33: //LD B, Vx
                    buffer = chip->V[x];
                    for(int i = 0; i < 3; i++) {
                        writeMemory(chip, chip->I + (2 - i), buffer % 10);
                        buffer /= 10;
                    }
                    break;
                case 0x55: //LD [I], Vx
                    for(int i = 0; i <= x; i++) {
                        writeMemory(chip, chip->I + i, chip->V[i]);
                    }
                    if (MEMORY) { writeI(chip, chip->I + x + 1); }
                    break;
                case 0x65: //LD Vx, [I]
                    for(int i = 0; i <= x; i++) {
                        chip->V[i] = readMemory(chip, chip->I + i);
                    }
                    if (MEMORY) { writeI(chip, chip->I + x + 1); }
                    break;
            }
            break;
    }
    return SUCCESS;
}

void draw(chip8 *chip, byte x, byte y, byte size) {
    x = x % SCREEN_X;
    y = y % SCREEN_Y;
    chip->V[0xF] = 0;

    for(byte i = 0; i < size; i++) {
        byte spriteByte = chip->memory[(chip->I+i) & 0x0FFF];
        if (y + i == SCREEN_Y && CLIPPING) { break; }
        byte localY = (y + i) % SCREEN_Y;

        for(byte j = 0; j < 8; j++) {
            if (x + j == SCREEN_X && CLIPPING) { break; }
            byte localX = (x + j) % SCREEN_X;

            if((spriteByte >> (7 - j)) & 1) {
                chip->V[0xF] = chip->V[0xF] | chip->screen[localY][localX];
                chip->screen[localY][localX] ^= 1;
            }
        }
    }
}

chip8* initChip(const char *rom_path) {
    chip8* chip = createChip();
    byte buffer[0xE00];
    FILE* file = fopen(rom_path, "rb");
    int size = fread(&buffer, 1, 0xE00, file);
    fclose(file);
    writeROM(chip, buffer, size);
    return chip;
}

void updateTimers(chip8 *chip) {
    if (chip->DT) {
        chip->DT -= 1;
    }
    if (chip->ST) {
        chip->ST -= 1;
    }
}

void writeMemory(chip8 *chip, word address, byte value) {
    chip->memory[address & 0x0FFF] = value;
}

byte readMemory(chip8 *chip, word address) {
    return chip->memory[address & 0x0FFF];
}

void writeI(chip8* chip, word value) {
    chip->I = value & 0x0FFF;
}



