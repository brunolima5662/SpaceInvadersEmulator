#include <stdint.h>
#include "machine.h"

#ifndef EMULATOR
#define EMULATOR

uint8_t emulate_next_instruction(machine_t *);
void interrupt_cpu(machine_t *, uint8_t);

#endif
