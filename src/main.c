#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "machine.h"
#include "disassembler.h"

int main(int argc, char * argv[]) {

    // read rom file into memory
    FILE * rom = fopen("rom/rom", "rb");
    if (rom == NULL) {
        printf("ROM file not found, exiting...");
        exit(1);
    }

    // fetch the file size
    fseek(rom, 0, SEEK_END);
    uint16_t size = ftell(rom);
    fseek(rom, 0, SEEK_SET);

    // allocate memory to hold the rom's data
    unsigned char * rom_mem = malloc(size);

    // read the rom file into the allocated memory
    // and close it when done
    fread(rom_mem, size, 1, rom);
    fclose(rom);

    // disassemble rom into assembly code
    uint16_t num_parsed = disassemble(rom_mem, size);

    // display number of parsed instructions
    printf("\nPARSED %d INSTRUCTIONS\n", num_parsed);

    // deallocate rom memory
    free(rom_mem);

    return 0;
}
