#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "machine.h"
#include "emulator.h"
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

    // declare and initialize the machine object
    machine_t machine;
    initialize_machine(&machine);

    // read the rom file into the machine's
    // memory and close it when done
    fread(&machine.memory[0], size, 1, rom);
    fclose(rom);

    // start the emulation loop
    uint8_t done = 0;
    while(done == 0) {
        if(checkMachineInstruction(&machine) == 0) {
            // check if the next intruction should be handled by
            // special machine hardware. if not (returns 0), pass it
            // on to be processed by the cpu emulator
            done = emulate_next_instruction(&machine);
        }

        if(time(NULL) - machine.last_rendered >= VIDEO_HZ) {
            // render next video frame and generate
            // cpu interrupts to go along with it
            interrupt_cpu(&machine, 2);
            machine.last_rendered = time(NULL);
        }
    }

    // disassemble rom into assembly code
    //uint16_t num_parsed = disassemble(rom_mem, size);

    // display number of parsed instructions
    //printf("\nPARSED %d INSTRUCTIONS\n", num_parsed);

    return 0;
}
