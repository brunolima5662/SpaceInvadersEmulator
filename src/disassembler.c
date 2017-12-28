#include <stdint.h>
#include <stdio.h>
#include <string.h>

int disassemble(unsigned char * rom, uint16_t size) {
    uint16_t counter = 0;
    while(counter < size) {
        switch(rom[counter]) {
            case 0x00: printf("NOP\n"); break;
            default: printf("UNSUPPORTED OP\n"); break;
        }
        counter += 1;
    }
    return counter;
}
