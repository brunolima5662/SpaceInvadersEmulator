#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "SDL2/SDL.h"
#include "machine.h"
#include "emulator.h"
#include "disassembler.h"

uint32_t mcs_per_frame = ((1.0f / VIDEO_HZ) * 1000000UL);


int main(int argc, char * argv[]) {
    uint8_t done = 0;
    SDL_Window * window = NULL;
    SDL_Surface * screen = NULL;
    SDL_Event evt;

    // start SDL library and main window
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }
    window = SDL_CreateWindow(
        "Space Invaders",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        VIDEO_Y * VIDEO_SCALE, // flip width and height since the machine's screen
        VIDEO_X * VIDEO_SCALE, // is actually flipped on its side for it to be vertical
        SDL_WINDOW_SHOWN
    );
    screen = SDL_GetWindowSurface(window);
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));
    SDL_UpdateWindowSurface(window);

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
    uint64_t cycles = 0;
    uint8_t interrupt = 1, frame_ms_offset = 0;
    struct timespec cpu_timer;
    uint64_t cpu_time_start = 0, cpu_time_delta = 0;

    while(done == 0) {
        cycles = 0;

        // store the cpu start time for this millisecond batch of operations
        clock_gettime(CLOCK_REALTIME, &cpu_timer);
        cpu_time_start = cpu_timer.tv_nsec;

        // renders new frame if any
        if(frame_ms_offset == 16) {
            render_frame(&machine, screen);
            SDL_UpdateWindowSurface(window);
        }



        // process interrupts if necessary
        if(frame_ms_offset == 8 || frame_ms_offset == 16) {
            if(machine.accept_interrupt == 1) {
                //printf("Processing interrupt: %d\n", interrupt);
                interrupt_cpu(&machine, interrupt);
            }
            interrupt ^= 0x03; // toggle between interrupts 1 and 2
        }


        while(cycles < CLOCK_CYCLES_PER_MS) {
            // add opcode's cycle number to cycles accumulator
            cycles += clock_cycles[machine.memory[machine.pc]];

            // check if the next intruction should be handled by
            // special machine hardware. if not (returns 0), pass it
            // on to be processed by the cpu emulator
            // disassemble_at_memory(machine.memory, machine.pc);
            if(check_machine_instruction(&machine) == 0) {
                done = !emulate_next_instruction(&machine);
            }
        }

        // once the emulator has processed the amount of clock cycles per
        // millisecond, sleep until it's time to process the next millisecond
        // batch of clock cycles
        if(frame_ms_offset >= 16)
            frame_ms_offset = 1;
        else
            frame_ms_offset += 1;
        clock_gettime(CLOCK_REALTIME, &cpu_timer);
        if(cpu_timer.tv_nsec >= cpu_time_start)
            cpu_time_delta = (cpu_timer.tv_nsec - cpu_time_start) / 1000;
        else
            cpu_time_delta = (1000000000UL - cpu_time_start + cpu_timer.tv_nsec) / 1000;
        if(cpu_time_delta < 1000) {
            sleep_microseconds(1000 - cpu_time_delta);
        }

        while(SDL_PollEvent(&evt)) {
            switch(evt.type) {
                case SDL_QUIT: done = 1; break;
            }
        }
    }

    // quit SDL
    SDL_DestroyWindow(window);
    SDL_Quit();


    printf("Finished\n");

    return 0;
}
