#include <stdint.h>

#ifndef MACHINE
#define MACHINE

#define MEMORY_SIZE 65536 // in bytes
#define VIDEO_X 256 // in pixels
#define VIDEO_Y 224 // in pixes
#define VIDEO_SCANLINE 32 // in bytes
#define VIDEO_HZ 60 // in Hz

#define ROM_START 0x0000
#define RAM_START 0x2000
#define VIDEO_START 0x2400
#define RAM_MIRROR_START 0x4000

typedef struct machine {
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
    uint16_t pc;
    uint16_t sp;
    uint8_t z;
    uint8_t s;
    uint8_t p;
    uint8_t ac;
    uint8_t cy;
    unsigned char memory[MEMORY_SIZE];
} machine_t;

#endif
