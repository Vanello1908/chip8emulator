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

byte isKeyPressed(byte key) {
    //TODO: key pressed
    char ckey = '0';
    if(key < 0xA){
        ckey = key + 0x30;
    }
    else {
        ckey = key - 0xA + 0x41;
    }
    SDL_Event event;
    return 0x0;
}
