#include <stdint.h>
#include <time.h>
#include <unistd.h>

#ifndef MACHINE
#define MACHINE

#define MEMORY_SIZE 0x10000
#define ROM_START 0x0000
#define RAM_START 0x2000
#define VIDEO_RAM_START 0x2400
#define RAM_MIRROR_START 0x4000

#define VIDEO_X 256 // in pixels
#define VIDEO_Y 224 // in pixels
#define VIDEO_SCANLINE 32 // in bytes
#define VIDEO_HZ 0.0166666667f // in seconds ( 60 Hz )
#define CPU_HZ 0.5 // in nanoseconds ( 2 MHz )
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

    // time counters to control video rendering
    // and processing speeds
    time_t last_rendered;
    long   last_processed;
} machine_t;

void initialize_machine(machine_t *);
int checkMachineInstruction(machine_t *);

#endif
