#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "machine.h"
#include "emulator.h"
#include "disassembler.h"

void get_current_nanoseconds(long *);

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
    long process_time = 0;
    long process_time_delta = 0;
    while(done == 0) {

        // control emulation frequency to not exceed 8080's frequency (2 MHz)
        get_current_nanoseconds(&process_time);
        process_time_delta = process_time - machine.last_processed;
        if(process_time_delta < CPU_HZ)
            sleep_nanoseconds(CPU_HZ - process_time_delta);

        // check if the next intruction should be handled by
        // special machine hardware. if not (returns 0), pass it
        // on to be processed by the cpu emulator
        if(check_machine_instruction(&machine) == 0) {

            done = emulate_next_instruction(&machine);
        }

        // check if it's time to render the next video frame
        if(time(NULL) - machine.last_rendered >= VIDEO_HZ) {
            // render next video frame and generate
            // cpu interrupts to go along with it
            interrupt_cpu(&machine, 2);
            machine.last_rendered = time(NULL);
        }

        // update machine's last processed timestamp
        get_current_nanoseconds(&process_time);
        machine.last_processed = process_time;
    }

    // disassemble rom into assembly code
    //uint16_t num_parsed = disassemble(rom_mem, size);

    // display number of parsed instructions
    //printf("\nPARSED %d INSTRUCTIONS\n", num_parsed);

    return 0;
}

void get_current_nanoseconds(long * container) {
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    (*container) = (spec.tv_sec * 1000000) + spec.tv_nsec;
}
