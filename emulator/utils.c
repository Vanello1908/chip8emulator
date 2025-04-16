#include "utils.h"

#include <SDL2/SDL_events.h>

word parseWord(byte byte1, byte byte2) {
    return (byte1 << 8) | byte2;
}

word getAddress(word instruction) {
    return instruction & 0xFFF;
}

byte getByte(word instruction) {
    return instruction & 0xFF;
}

byte getNibble(word instruction) {
    return instruction & 0xF;
}

byte getX(word instruction) {
    return (instruction >> 8) & 0xF;
}

byte getY(word instruction) {
    return (instruction >> 4) & 0xF;
}

byte keyToByte(const char* key) {
    if(strlen(key) != 1) {
        return 0x10;
    }
    char key_sym = *key;
    if (key_sym >= 0x30 && key_sym <= 0x39) {
        return (byte)key_sym - 0x30;
    }
    if (key_sym >= 0x41 && key_sym <= 0x46) {
        return (byte)key_sym - 0x41 + 0xA;
    }
    return 0x10;
}
