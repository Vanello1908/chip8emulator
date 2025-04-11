#include "utils.h"

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
