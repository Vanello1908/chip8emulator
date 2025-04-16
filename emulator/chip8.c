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
    chip->PC = 0x200;
    chip->SP = 0xFFFF;
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

    byte buffer = 0;;

    switch(instruction >> 12) {
        case 0x0:
            if(instruction == 0x00E0) { //CLS
                clearDisplay(chip);
            } else if(instruction == 0x00EE) { //RET
                if(chip->SP != 0xFFFF) {
                    chip->PC = chip->stack[chip->SP];
                    chip->SP--;
                }
                else {
                    return ERROR;
                }
            }
            break;

        case 0x1: //JP addr
            chip->PC = nnn;
            break;

        case 0x2: //CALL addr
            if(chip->SP != 0xF) {
                chip->SP++;
                chip->stack[chip->SP] = chip->PC;
                chip->PC = nnn;
            }
            else {
                return ERROR;
            }
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
            if(chip->V[x] == chip->V[y]) {
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
                    if(COPY_Y_ON_SHIFTING) {
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
                    if(COPY_Y_ON_SHIFTING) {
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
            word address = nnn;
            if(BXNN_JUMP) {
                address += chip->V[x];
            } else {
                address += chip->V[0x0];
            }
            chip->PC = address;
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
                    if(chip->V[x] < 0x10 && chip->keys[chip->V[x]] == 0x1) {
                        chip->PC += 2;
                    }
                    break;
                case 0xA1: //SKNP Vx
                    if(chip->V[x] < 0x10 && chip->keys[chip->V[x]] == 0x0) {
                        chip->PC += 2;
                    }
                    break;
            }
            break;
        case 0xF:
            //TODO: create sound
            byte end = nn;
            switch (end) {
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
                    chip->I += chip->V[x];
                    break;
                case 0x29: //LD F, Vx
                    //TODO: interpreter font digits
                    break;
                case 0x33: //LD B, Vx
                    if(chip->I + 2 > 0xFFF) {
                        return ERROR;
                    }
                    byte num = chip->V[x];
                    for(int i = 0; i < 3; i++) {
                        chip->memory[chip->I + (2 - i)] = num % 10;
                        num /= 10;
                    }
                    break;
                case 0x55: //LD [I], Vx
                    if(chip->I + x > 0xFFF) {
                        return ERROR;
                    }
                    for(int i = 0; i <= x; i++) {
                        chip->memory[chip->I + i] = chip->V[i];
                    }
                    if (MEMORY) {
                        chip->I += x + 1;
                    }
                    break;
                case 0x65: //LD Vx, [I]
                    byte index2 = x;
                    if(chip->I + index2 > 0xFFF) {
                        return ERROR;
                    }
                    for(int i = 0; i <= index2; i++) {
                        chip->V[i] = chip->memory[chip->I + i];
                    }
                    if (MEMORY) {
                        chip->I += x + 1;
                    }
                    break;
            }
            break;
    }
    return SUCCESS;
}

void draw(chip8 *chip, byte x, byte y, byte size) {
    byte flipResult = 0;
    for(byte i = 0; i < size; i++) {
        byte spriteByte = chip->memory[chip->I+i];
        byte localY = (y + i) % SCREEN_Y;
        for(byte j = 0; j < 8; j++) {
            byte localX = (x + (7 - j)) % SCREEN_X;
            byte newPixel = spriteByte & 0b1;
            if(newPixel) {
                chip->screen[localY][localX] ^= newPixel;
                if(chip->screen[localY][localX] == 0) {
                    flipResult = 1;
                }
            }
            spriteByte >>= 1;
        }
    }
    chip->V[0xF] = flipResult;

}

word startChip(chip8 *chip) {
    while (executeInstruction(chip) == SUCCESS && chip->PC != 0x00) {}
    if(chip->PC != 0x00) {
        return chip->PC;
    }
    return 0;
}

chip8* initChip(const char *rom_path) {
    chip8* chip = createChip();
    byte buffer[0xE00];
    FILE* file = fopen(rom_path, "rb");
    int size = fread(&buffer, 1, 0xE00, file);
    fclose(file);
    writeROM(chip, &buffer, size);
    return chip;
}

void updateTimers(chip8 *chip) {
    if (chip->DT > 0x0) {
        chip->DT -= 1;
    }
    if (chip->ST > 0x0) {
        chip->ST -= 1;
    }
}


