#ifndef EMULATOR
#define EMULATOR

#include <stdint.h>
#include "machine.h"

uint8_t emulate_next_instruction(machine_t *);

#ifndef __EMSCRIPTEN__
// the following array holds the number of clock cycles it
// takes to run the opcode of its index.
// e.g clock_cycles[0xd4] = 17 means it takes seventeen
// clock cycles to run opcode 0xd4 in the 8080 cpu.
uint8_t clock_cycles[0x100];
#endif

#endif
