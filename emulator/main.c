#include <stdio.h>
#include <stdlib.h>

#include "chip8.h"
#include "definitions.h"

int main(void) {
    byte a = 1;
    byte b = 2;
    word c = (word)(a << 8) | b;
    printf("%x\n", c);
}
