#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include "machine.h"
#include "emulator.h"
#include "disassembler.h"

int main(int argc, char * argv[]) {
    uint8_t done = 0, paused = 0;
    int volume = -1;
    FILE * rom = NULL;
    SDL_Event      evt;
    SDL_Window   * window    = NULL;
    SDL_Renderer * renderer  = NULL;
    SDL_Surface  * screen    = NULL;
    SDL_Texture  * texture   = NULL;

    // start SDL library and main window
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    if(Mix_OpenAudio(11025, AUDIO_U8, 1, 512) < 0) {
        SDL_Quit();
        fprintf(stderr, "SDL_mixer Initialization Error: %s\n", Mix_GetError());
        exit(EXIT_FAILURE);
    }

    // declare and initialize machine
    machine_t machine;
    initialize_machine(&machine);

    // read rom file into memory
    if(argc > 1) {
        rom = fopen(argv[1], "rb");
        if (rom == NULL) {
            fprintf(stderr, "ROM file invalid, exiting...");
            Mix_Quit();
            SDL_Quit();
            exit(EXIT_FAILURE);
        }
    }
    else {
        fprintf(stderr, "No rom file specified, exiting...\n");
        fprintf(stderr, "Usage: %s <path to rom file...>\n", argv[0]);
        Mix_Quit();
        SDL_Quit();
        exit(EXIT_FAILURE);
    }


    // fetch the file size
    fseek(rom, 0, SEEK_END);
    uint16_t size = ftell(rom);
    fseek(rom, 0, SEEK_SET);

    // load rom into machine's ram
    fread(&machine.memory[0], size, 1, rom);
    fclose(rom);


    // initialize SDL window and screen
    window = SDL_CreateWindow(
        "Space Invaders",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        VIDEO_Y * VIDEO_SCALE, // flip width and height since the machine's screen
        VIDEO_X * VIDEO_SCALE, // is actually flipped on its side for it to be vertical
        SDL_WINDOW_SHOWN
    );
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    screen = SDL_CreateRGBSurfaceWithFormat(0, VIDEO_Y, VIDEO_X, 8, SDL_PIXELFORMAT_RGB332);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STREAMING, VIDEO_Y, VIDEO_X);

    if(!texture || !renderer) {
        if(!texture)
            fprintf(stderr, "Texture Error: %s\n", Mix_GetError());
        if(!renderer)
            fprintf(stderr, "Renderer Error: %s\n", Mix_GetError());
        SDL_DestroyWindow(window);
        Mix_Quit();
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    // start the emulation loop
    uint32_t cycles = 0;
    uint16_t ms_per_frame = (uint32_t)((1.0f / VIDEO_HZ) * 1000);
    uint32_t cycles_per_frame = ms_per_frame * CPU_KHZ;
    uint16_t cycles_per_interrupt = cycles_per_frame / 2;
    uint8_t interrupt = 1;

    // all time delta calculations are done in microseconds
    struct timespec cpu_timer;
    uint64_t cpu_time_start = 0, cpu_time_delta = 0;
    clock_gettime(CLOCK_REALTIME, &cpu_timer);
    cpu_time_start = cpu_timer.tv_nsec;

    // the following loop runs once per frame (~16 ms)
    while(done == 0) {

        if(!paused) {
            // render next frame
            render_screen(&machine, screen);
            SDL_UpdateTexture(texture, NULL, screen->pixels, screen->pitch);
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }

        // if the last batch of cycles and rendering took less than the
        // amount of time between frames (~16 ms), then sleep for the
        // remainder of that time
        clock_gettime(CLOCK_REALTIME, &cpu_timer);
        if(cpu_timer.tv_nsec >= cpu_time_start)
            cpu_time_delta = (cpu_timer.tv_nsec - cpu_time_start) / 1000;
        else
            cpu_time_delta = (1000000000UL - cpu_time_start + cpu_timer.tv_nsec) / 1000;
        if(cpu_time_delta < 16000)
            sleep_microseconds(16000 - cpu_time_delta);

            // store current time to calculate the processing time delta
            // during the next frame render
            clock_gettime(CLOCK_REALTIME, &cpu_timer);
            cpu_time_start = cpu_timer.tv_nsec;

        if(!paused) {
            
            // perform a frame's worth of cpu operations...
            cycles = 0;
            while(cycles < cycles_per_frame) {

                // add opcode's cycle number to cycles accumulator
                cycles += clock_cycles[machine.memory[machine.pc]];

                // check if the next intruction should be handled by
                // special machine hardware. if not (returns 0), pass it
                // on to be processed by the cpu emulator
                // disassemble_at_memory(machine.memory, machine.pc);
                if(check_machine_instruction(&machine) == 0) {
                    done = !emulate_next_instruction(&machine);
                }

                if(interrupt == 1 && cycles >= cycles_per_interrupt) {
                    // process first interrupt at around half way through
                    // the cycles per frame
                    if(machine.accept_interrupt == 1) {
                        interrupt_cpu(&machine, interrupt);
                        interrupt ^= 0x03; // toggle between interrupts 1 and 2
                    }
                }
            }
            // process second interrupt
            if(machine.accept_interrupt == 1) {
                interrupt_cpu(&machine, interrupt);
                interrupt ^= 0x03; // toggle between interrupts 1 and 2
            }
        }

        // check for new key events...
        SI_KEY_RESULT key_result;
        while(SDL_PollEvent(&evt)) {
            switch(evt.type) {
                case SDL_QUIT: done = 1; break;
                case SDL_KEYUP:
                case SDL_KEYDOWN:
                    key_result = handle_input(&machine, evt.type, evt.key.keysym.sym);
                    done |= (key_result == SI_KEY_RESULT_EXIT);
                    if(key_result == SI_KEY_RESULT_TOGGLE_MUTE && evt.type == SDL_KEYUP) {
                        if(volume < 0) {
                            volume = Mix_Volume(-1, -1);
                            Mix_Volume(-1, 0);
                        }
                        else {
                            Mix_Volume(-1, volume);
                            volume = -1;
                        }
                    }
                    if(key_result == SI_KEY_RESULT_PAUSE && evt.type == SDL_KEYUP)
                        paused ^= 0x01;
                    break;
                default: ;
            }
        }
    }

    // shutdown machine
    shutdown_machine(&machine);

    // quit SDL
    SDL_FreeSurface(screen);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_Quit();
    SDL_Quit();


    printf("Finished\n");

    return 1;
}
