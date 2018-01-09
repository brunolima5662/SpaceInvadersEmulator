#ifndef MACHINE
#define MACHINE

#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "SDL2/SDL.h"

#define MEMORY_SIZE 0x10000
#define ROM_START 0x0000
#define RAM_START 0x2000
#define VIDEO_RAM_START 0x2400
#define RAM_MIRROR_START 0x4000

#define VIDEO_SCALE 3
#define VIDEO_X 256 // in pixels
#define VIDEO_Y 224 // in pixels
#define VIDEO_SCANLINE 32 // in bytes
#define VIDEO_HZ 60 // in Hz
#define CPU_HZ 2000000 // in Hz
#define CLOCK_CYCLES_PER_MS (CPU_HZ / 1000)
#define IO_PORTS 8

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
    uint8_t ports[IO_PORTS];

    // shift operation hardware specific to
    // the space invaders machine (not part
    // of the 8080 cpu)
    uint8_t shift_hi;
    uint8_t shift_lo;
    uint8_t shift_offset;

    // interrupt flag
    uint8_t accept_interrupt;
} machine_t;

void initialize_machine(machine_t *);
int check_machine_instruction(machine_t *);
void render_frame(machine_t *, SDL_Surface *);
void sleep_microseconds(uint64_t);
void interrupt_cpu(machine_t *, uint8_t);

#endif
