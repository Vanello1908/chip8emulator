#ifndef UTILS_H
#define UTILS_H

#include "definitions.h"
word parseWord(byte byte1, byte byte2);
word getAddress(word instruction);
byte getByte(word instruction);
byte getNibble(word instruction);
byte getX(word instruction);
byte getY(word instruction);
byte isKeyPressed(byte key);
#endif
