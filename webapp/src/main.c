#include <stdint.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <emscripten.h>
#include "native/machine.h"
#include "native/emulator.h"
#include "native/disassembler.h"

typedef struct cpu_context {
    machine_t    *  machine;
    SDL_Renderer *  renderer;
    SDL_Surface  *  screen;
    SDL_Texture  *  texture;
    SDL_Event    *  event;
    uint8_t         paused;
    uint8_t         volume;
    uint8_t         interrupt;
    uint32_t        cycles;
    uint32_t        cycles_per_frame;
    uint16_t        cycles_per_interrupt;
    uint8_t         done;
} cpu_context_t;

void cpu_run(void *);

int mainf(int argc, char * argv[]) {
    uint8_t done = 0, paused = 0;
    int volume = -1;
    FILE * rom = NULL;
    SDL_Event       evt;
    SDL_DisplayMode displayMode;
    SDL_Window   * window    = NULL;
    SDL_Renderer * renderer  = NULL;
    SDL_Surface  * screen    = NULL, * r_screen = NULL;
    SDL_Texture  * texture   = NULL;
    SDL_Rect     screen_rect = { 0, 0, VIDEO_Y * VIDEO_SCALE, VIDEO_X * VIDEO_SCALE };

    // get device's screen resolution
    if(SDL_GetCurrentDisplayMode(0, &displayMode) == 0) {
        screen_rect.w = displayMode.w;
        screen_rect.h = displayMode.h;
    }

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
        rom = fopen("assets/ROM", "rb");
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


    // initialize SDL window, renderer, and texture
    window = SDL_CreateWindow(
        "Space Invaders",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        screen_rect.w,
        screen_rect.h,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    texture  = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STREAMING, VIDEO_Y, VIDEO_X);


    // create SDL surface weith pixel format as RGB332
    r_screen = SDL_CreateRGBSurface(0, VIDEO_Y, VIDEO_X, 8, 0, 0, 0, 0);
    screen   = SDL_ConvertSurfaceFormat(r_screen, SDL_PIXELFORMAT_RGB332, 0);
    SDL_FreeSurface(r_screen);

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

    // create a context object with all resources that the
    // cpu emulator loop will need
    cpu_context_t context;
    context.machine = &machine;
    context.renderer = renderer;
    context.screen = screen;
    context.texture = texture;
    context.paused = 0;
    context.volume = 0;
    context.interrupt = 1;
    context.cycles = 0;
    context.cycles_per_frame = (uint32_t)((1.0f / VIDEO_HZ) * 1000) * CPU_KHZ;
    context.cycles_per_interrupt = context.cycles_per_frame / 2;
    context.done = 0;

    // start web-friendly infitite loop through emscripten's api
    emscripten_set_main_loop_arg(cpu_run, &context, -1, 1);

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

// run a single cpu instruction
void cpu_run(void * arg) {
    cpu_context_t * ctx = (cpu_context_t *)arg;
    if(!ctx->paused) {
        // render next frame
        render_screen(ctx->machine, ctx->screen);
        SDL_UpdateTexture(ctx->texture, NULL, ctx->screen->pixels, ctx->screen->pitch);
        SDL_RenderClear(ctx->renderer);
        SDL_RenderCopy(ctx->renderer, ctx->texture, NULL, NULL);
        SDL_RenderPresent(ctx->renderer);
    }

    if(!ctx->paused) {

        // perform a frame's worth of cpu operations...
        ctx->cycles = 0;
        while(ctx->cycles < ctx->cycles_per_frame) {

            // add opcode's cycle number to cycles accumulator
            ctx->cycles += clock_cycles[ctx->machine->memory[ctx->machine->pc]];

            // check if the next intruction should be handled by
            // special machine hardware. if not (returns 0), pass it
            // on to be processed by the cpu emulator
            // disassemble_at_memory(machine.memory, machine.pc);
            if(check_machine_instruction(ctx->machine) == 0) {
                ctx->done = !emulate_next_instruction(ctx->machine);
            }

            if(ctx->interrupt == 1 && ctx->cycles >= ctx->cycles_per_interrupt) {
                // process first interrupt at around half way through
                // the cycles per frame
                if(ctx->machine->accept_interrupt == 1) {
                    interrupt_cpu(ctx->machine, ctx->interrupt);
                    ctx->interrupt ^= 0x03; // toggle between interrupts 1 and 2
                }
            }

            if(ctx->done)
                break;
        }
        // process second interrupt
        if(ctx->machine->accept_interrupt == 1) {
            interrupt_cpu(ctx->machine, ctx->interrupt);
            ctx->interrupt ^= 0x03; // toggle between interrupts 1 and 2
        }
    }

    // check for new key events...
    SI_KEY_RESULT key_result;
    while(SDL_PollEvent(ctx->event)) {
        switch(ctx->event->type) {
            case SDL_QUIT: ctx->done = 1; break;
            case SDL_KEYUP:
            case SDL_KEYDOWN:
                key_result = handle_input(ctx->machine, ctx->event->type, ctx->event->key.keysym.sym);
                ctx->done |= (key_result == SI_KEY_RESULT_EXIT);
                if(key_result == SI_KEY_RESULT_TOGGLE_MUTE && ctx->event->type == SDL_KEYUP) {
                    if(ctx->volume < 0) {
                        ctx->volume = Mix_Volume(-1, -1);
                        Mix_Volume(-1, 0);
                    }
                    else {
                        Mix_Volume(-1, ctx->volume);
                        ctx->volume = -1;
                    }
                }
                if(key_result == SI_KEY_RESULT_PAUSE && ctx->event->type == SDL_KEYUP)
                    ctx->paused ^= 0x01;
                break;
            default: ;
        }
    }

    if(ctx->done)
        emscripten_cancel_main_loop();
}
