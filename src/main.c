#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "SDL/SDL.h"
#include "machine.h"
#include "emulator.h"
#include "disassembler.h"

long get_ms();

int main(int argc, char * argv[]) {
    SDL_Surface * screen = NULL;

    // read rom file into memory
    FILE * rom = fopen("rom/rom", "rb");
    if (rom == NULL) {
        printf("ROM file not found, exiting...");
        exit(1);
    }

    // start SDL library and main screen
    SDL_Init(SDL_INIT_EVERYTHING);
    screen = SDL_SetVideoMode( VIDEO_X, VIDEO_Y, 32, SDL_SWSURFACE );

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
    uint16_t cycles = 0;
    long process_time_delta = 0;
    machine.last_rendered = get_ms();
    while(done == 0) {

        // add opcode's cycle number to cycles accumulator
        cycles += clock_cycles[machine.memory[machine.pc]];

        // check if the next intruction should be handled by
        // special machine hardware. if not (returns 0), pass it
        // on to be processed by the cpu emulator
        if(check_machine_instruction(&machine) == 0) {
            done = emulate_next_instruction(&machine);
        }

        // once the emulator has processed the amount of clock cycles per
        // frame, sleep until it's time to render the next frame
        if(cycles >= CLOCK_CYCLES_PER_FRAME) {
            cycles = 0;
            process_time_delta = get_ms() - machine.last_rendered;
            sleep_milliseconds(MS_PER_FRAME - process_time_delta);

            // render next video frame and generate
            // cpu interrupts to go along with it
            interrupt_cpu(&machine, 2);

            // update machine's last rendered timestamp
            machine.last_rendered = get_ms();
        }

    }

    // disassemble rom into assembly code
    //uint16_t num_parsed = disassemble(rom_mem, size);

    // display number of parsed instructions
    //printf("\nPARSED %d INSTRUCTIONS\n", num_parsed);

    // quit SDL
    SDL_Quit();

    return 0;
}

void get_ms() {
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    return (spec.tv_sec * 1000) + (spec.tv_nsec / 1000);
}
