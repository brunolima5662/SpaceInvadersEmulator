#include <stdint.h>
#include "machine.h"

#ifndef EMULATOR
#define EMULATOR

uint8_t emulate_next_instruction(machine_t *);
void interrupt_cpu(machine_t *, uint8_t);

// the following array holds the number of clock cycles it
// takes to run the opcode of its index.
// e.g clock_cycles[0xd4] = 17 means it takes seventeen
// clock cycles to run opcode 0xd4 in the 8080 cpu.
uint8_t clock_cycles[0x100] = {
	4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4,            /* 0x00 - 0x0f */
	4, 10, 7, 5, 5, 5, 7, 4, 4, 10, 7, 5, 5, 5, 7, 4,            /* 0x10 - 0x1f */
	4, 10, 16, 5, 5, 5, 7, 4, 4, 10, 16, 5, 5, 5, 7, 4,          /* 0x20 - 0x2f */
	4, 10, 13, 5, 10, 10, 7, 4, 4, 10, 13, 5, 5, 5, 7, 4,        /* 0x30 - 0x3f */
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,              /* 0x40 - 0x4f */
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,              /* 0x50 - 0x5f */
	5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 7, 5,              /* 0x60 - 0x6f */
	7, 7, 7, 7, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 7, 5,              /* 0x70 - 0x7f */
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,              /* 0x80 - 0x8f */
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,              /* 0x90 - 0x9f */
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,              /* 0xa0 - 0xaf */
	4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 7, 4,              /* 0xb0 - 0xbf */
	11, 10, 10, 10, 17, 11, 7, 11, 11, 10, 10, 4, 17, 17, 7, 11, /* 0xc0 - 0xcf */
	11, 10, 10, 10, 17, 11, 7, 11, 11, 4, 10, 10, 17, 4, 7, 11,  /* 0xd0 - 0xdf */
	11, 10, 10, 18, 17, 11, 7, 11, 11, 5, 10, 4, 17, 4, 7, 11,   /* 0xe0 - 0xef */
	11, 10, 10, 4, 17, 11, 7, 11, 11, 5, 10, 4, 17, 4, 7, 11     /* 0xf0 - 0xff */
};

#endif
