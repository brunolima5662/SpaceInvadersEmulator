#include <stdint.h>
#include "machine.h"

#ifndef EMULATOR
#define EMULATOR

uint8_t emulate_instruction(machine_t *, uint16_t *);

#endif
