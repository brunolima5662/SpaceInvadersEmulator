#include <stdint.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL/SDL_mixer.h>
#include <emscripten/emscripten.h>
#include "native/machine.h"
#include "native/emulator.h"
#include "native/disassembler.h"

typedef struct cpu_context {
    machine_t    *  machine;
    SDL_Window   *  window;
    SDL_Renderer *  renderer;
    SDL_Surface  *  screen;
    SDL_Texture  *  texture;
    SDL_Event       event;
    uint8_t         running;
    uint8_t         paused;
    uint8_t         volume;
    uint8_t         interrupt;
    uint32_t        cycles;
    uint32_t        cycles_per_frame;
    uint16_t        cycles_per_interrupt;
    uint8_t         foreground;
    uint8_t         background;
    uint8_t         done;
} cpu_context_t;

void cpu_run(void *);
void quit_sdl();

// the struct will hold and references and flags
// that the emulator loop will need
cpu_context_t context;

int main(int argc, char * argv[]) {
    return 0;
}

int EMSCRIPTEN_KEEPALIVE mainf(
    uint8_t set_colors_mask, // 0x1 = bcolor, 0x2 = fcolor, 0x3 = both
    uint8_t fcolor,
    uint8_t bcolor,
    uint8_t lives,
    uint8_t * saved_state,
    uint8_t * saved_machine
) {
    uint8_t done = 0, paused = 0;
    uint8_t corrected_lives  = (lives >= 3 && lives <= 6) ? lives : 3;
    int volume = -1;
    FILE * rom = NULL;
    SDL_Event       evt;
    SDL_DisplayMode displayMode;
    SDL_Window   * window    = NULL;
    SDL_Renderer * renderer  = NULL;
    SDL_Surface  * screen    = NULL, * r_screen = NULL;
    SDL_Texture  * texture   = NULL;
    SDL_Rect     screen_rect = { 0, 0, VIDEO_Y * VIDEO_SCALE, VIDEO_X * VIDEO_SCALE };

    // first, check that there are no emulation loop running.
    // if there are, cancel and close them first
    if(context.running) {
        emscripten_cancel_main_loop();
        quit_sdl();
        context.running = 0;
    }

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
    if(saved_state && saved_machine) {
        load_machine(&machine, saved_machine, saved_state);
    }
    else {
        if(saved_state)
            free(saved_state);
        if(saved_machine)
            free(saved_machine);

        initialize_machine(&machine, corrected_lives);

        // read rom file into memory
        rom = fopen("assets/ROM", "rb");
        if (rom == NULL) {
            fprintf(stderr, "ROM file invalid, exiting...\n");
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
    }


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
    screen   = SDL_CreateRGBSurfaceWithFormat(0, VIDEO_Y, VIDEO_X, 8, SDL_PIXELFORMAT_RGB332);
    texture  = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_STREAMING, VIDEO_Y, VIDEO_X);


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

    // setup initial state for the cpu context
    context.machine = &machine;
    context.window  = window;
    context.renderer = renderer;
    context.screen = screen;
    context.texture = texture;
    context.paused = 0;
    context.volume = 0;
    context.interrupt = 1;
    context.cycles = 0;
    context.cycles_per_frame = (uint32_t)((1.0f / VIDEO_HZ) * 1000) * CPU_KHZ;
    context.cycles_per_interrupt = context.cycles_per_frame / 2;
    context.running = 1;
    context.foreground = (set_colors_mask & 0x2) ? fcolor : FOREGROUND_COLOR;
    context.background = (set_colors_mask & 0x1) ? bcolor : BACKGROUND_COLOR;
    context.done = 0;

    // start web-friendly infitite loop through emscripten's api
    emscripten_set_main_loop_arg(cpu_run, (void *)&context, -1, 1);

    return 1;
}

// command from JS to stop emulation
void EMSCRIPTEN_KEEPALIVE halt() {
    emscripten_cancel_main_loop();
    quit_sdl();
    context.running = 0;
}

// command from pause / resume emulation
void EMSCRIPTEN_KEEPALIVE toggle_pause() {
    context.paused ^= 0x01;
}

uint32_t EMSCRIPTEN_KEEPALIVE get_state() {
    return (uint32_t)(&(context.machine->memory[0]));
}

uint32_t EMSCRIPTEN_KEEPALIVE get_machine() {
    uint8_t * machine = (uint8_t *)malloc(19 + (2 * IO_PORTS));
    machine[0] = context.machine->a;
    machine[1] = context.machine->b;
    machine[2] = context.machine->c;
    machine[3] = context.machine->d;
    machine[4] = context.machine->e;
    machine[5] = context.machine->h;
    machine[6] = context.machine->l;
    machine[7] = (uint8_t)(context.machine->pc >> 8);
    machine[8] = (uint8_t)(context.machine->pc & 0xff);
    machine[9] = (uint8_t)(context.machine->sp >> 8);
    machine[10] = (uint8_t)(context.machine->sp & 0xff);
    machine[11] = context.machine->z;
    machine[12] = context.machine->s;
    machine[13] = context.machine->p;
    machine[14] = context.machine->cy;
    machine[15] = context.machine->shift_hi;
    machine[16] = context.machine->shift_lo;
    machine[17] = context.machine->shift_offset;
    machine[18] = context.machine->accept_interrupt;
    memcpy(&machine[19], context.machine->ports_in, IO_PORTS);
    memcpy(&machine[19 + IO_PORTS], context.machine->ports_out, IO_PORTS);
    return (uint32_t)machine;
}

// cleanup after main loop finishes
void quit_sdl() {
    if(context.running) {
        SDL_FreeSurface(context.screen);
        SDL_DestroyTexture(context.texture);
        SDL_DestroyRenderer(context.renderer);
        SDL_DestroyWindow(context.window);
        Mix_Quit();
        SDL_Quit();
    }
}

// run a single cpu instruction
void cpu_run(void * arg) {
    cpu_context_t * ctx = (cpu_context_t *)arg;
    if(!ctx->paused) {
        // render next frame
        render_screen(ctx->machine, ctx->screen, ctx->foreground, ctx->background);
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
    SDL_Event * ev = &(ctx->event);
    while(SDL_PollEvent(ev)) {
        switch(ev->type) {
            case SDL_QUIT: ctx->done = 1; break;
            case SDL_KEYUP:
            case SDL_KEYDOWN:
                key_result = handle_input(ctx->machine, ev->type, ev->key.keysym.sym);
                ctx->done |= (key_result == SI_KEY_RESULT_EXIT);
                if(key_result == SI_KEY_RESULT_TOGGLE_MUTE && ev->type == SDL_KEYUP) {
                    if(ctx->volume < 0) {
                        ctx->volume = Mix_Volume(-1, -1);
                        Mix_Volume(-1, 0);
                    }
                    else {
                        Mix_Volume(-1, ctx->volume);
                        ctx->volume = -1;
                    }
                }
                if(key_result == SI_KEY_RESULT_PAUSE && ev->type == SDL_KEYUP)
                    ctx->paused ^= 0x01;
                break;
            default: ;
        }
    }
    if(ctx->done) {
        emscripten_cancel_main_loop();
        quit_sdl();
        EM_ASM(
            var stop = new Event("emulator_stop");
            document.dispatchEvent(stop);
        );
        context.running = 0;
    }
}
