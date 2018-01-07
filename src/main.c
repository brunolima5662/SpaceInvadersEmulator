#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include "SDL2/SDL.h"
#include "machine.h"
#include "emulator.h"
#include "disassembler.h"

uint64_t get_ms();

int main(int argc, char * argv[]) {
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
        VIDEO_Y, // flip width and height since the machine's screen
        VIDEO_X, // is actually flipped on its side for it to be vertical
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
    uint8_t done = 0;
    uint16_t cycles = 0;
    uint16_t half_cpf = CLOCK_CYCLES_PER_FRAME / 2;
    uint8_t processed_interrupt_1 = 0;
    uint32_t process_time_delta = 0;
    uint64_t count = 0;
    machine.last_rendered = get_ms();
    while(done == 0) {
        count += 1;
        // add opcode's cycle number to cycles accumulator
        cycles += clock_cycles[machine.memory[machine.pc]];

        // check if the next intruction should be handled by
        // special machine hardware. if not (returns 0), pass it
        // on to be processed by the cpu emulator
        // printf("%lu", count);
        // disassemble_at_memory(machine.memory, machine.pc);
        if(check_machine_instruction(&machine) == 0) {
            done = !emulate_next_instruction(&machine);
        }

        if(machine.accept_interrupt == 1 && processed_interrupt_1 == 0 && cycles >= half_cpf) {
            //printf("Emitting cpu interrupt 1...\n");
            processed_interrupt_1 = 1;
            interrupt_cpu(&machine, 1);
        }

        // once the emulator has processed the amount of clock cycles per
        // frame, sleep until it's time to render the next frame
        if(cycles >= CLOCK_CYCLES_PER_FRAME) {
            //printf("Hit cycles/frame threshold...\n");
            cycles = 0;
            processed_interrupt_1 = 0;
            process_time_delta = (uint32_t)(get_ms() - machine.last_rendered);
            if(process_time_delta < 17) {
                sleep_milliseconds(MS_PER_FRAME - process_time_delta);
            }

            //printf("Emitting cpu interrupt 2...\n");
            if(machine.accept_interrupt == 1) {
                interrupt_cpu(&machine, 2);
            }

            // printf("Rendering frame...\n");
            // render next video frame
            render_frame(&machine, screen);
            SDL_UpdateWindowSurface(window);

            // update machine's last rendered timestamp
            machine.last_rendered = get_ms();
        }

        while(SDL_PollEvent(&evt)) {
            switch(evt.type) {
                case SDL_QUIT: done = 1; break;
                /* process other events you want to handle here */
            }
        }

    }

    // quit SDL
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

uint64_t get_ms() {
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    uint64_t milliseconds = (spec.tv_sec - 1514764800) * 1000;
    return (milliseconds + (uint64_t)(spec.tv_nsec / 1000000UL));
}
