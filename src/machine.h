#ifndef MACHINE
#define MACHINE

#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#ifndef __EMSCRIPTEN__
#include <SDL.h>
#include <SDL_mixer.h>
#include "media.h"
#else
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#endif

#define MEMORY_SIZE 0x10000
#define ROM_START 0x0000
#define RAM_START 0x2000
#define VIDEO_RAM_START 0x2400
#define RAM_MIRROR_START 0x4000

#define BACKGROUND_COLOR 0x1f // in RGB332 format
#define FOREGROUND_COLOR 0xc1 // in RGB332 format
#define VIDEO_SCALE 3
#define VIDEO_X 256 // in pixels
#define VIDEO_Y 224 // in pixels
#define VIDEO_SIZE (VIDEO_X * VIDEO_Y) // in pixels
#define VIDEO_SCANLINE 32 // in bytes
#define VIDEO_HZ 60 // in hertz
#define CPU_KHZ 4000 // in kilohertz
#define IO_PORTS 8
#define SOUND_SAMPLES 9

#define GAME_NUMBER_OF_LIVES 3 // valid options: 3 - 6

typedef enum SI_KEY_RESULT {
    SI_KEY_RESULT_OK,
    SI_KEY_RESULT_EXIT,
    SI_KEY_RESULT_FULLSCREEN,
    SI_KEY_RESULT_TOGGLE_MUTE,
    SI_KEY_RESULT_PAUSE
} SI_KEY_RESULT;

// machine struct
typedef struct machine {
    uint8_t  a;
    uint8_t  b;
    uint8_t  c;
    uint8_t  d;
    uint8_t  e;
    uint8_t  h;
    uint8_t  l;
    uint16_t pc;
    uint16_t sp;
    uint8_t  z;
    uint8_t  s;
    uint8_t  p;
    uint8_t  cy;
    unsigned char memory[MEMORY_SIZE];
    uint8_t ports_in[IO_PORTS];
    uint8_t ports_out[IO_PORTS];

    // shift operation hardware specific to
    // the space invaders machine (not part
    // of the 8080 cpu)
    uint8_t shift_hi;
    uint8_t shift_lo;
    uint8_t shift_offset;

    // interrupt flag
    uint8_t accept_interrupt;

    // sound samples
    Mix_Chunk * samples[SOUND_SAMPLES];
    int32_t     channels[SOUND_SAMPLES];
} machine_t;

void initialize_machine(machine_t *);
void shutdown_machine(machine_t *);
int check_machine_instruction(machine_t *);
void render_screen(machine_t *, SDL_Surface *);
void sleep_microseconds(uint64_t);
void interrupt_cpu(machine_t *, uint8_t);
SI_KEY_RESULT handle_input(machine_t *, uint32_t, uint32_t);

#endif
