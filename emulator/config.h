#ifndef CONFIG_H
#define CONFIG_H
#include "definitions.h"

static const byte SCREEN_X = 0x40;
static const byte SCREEN_Y = 0x20;

static const byte SCREEN_COEFF = 8;
static const byte TICKS_PER_FRAME = 8;
static const byte FRAMES_PER_SECOND = 60;

static const byte LOGGING = 0;

// Quirks
static const byte SHIFTING = 1;
static const byte JUMPING = 0;
static const byte VF_RESET = 1;
static const byte MEMORY = 1;
static const byte CLIPPING = 1;
#endif
