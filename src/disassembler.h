#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef DISASSEMBLER
#define DISASSEMBLER

int disassemble_opcode(unsigned char *, uint16_t);
int disassemble(unsigned char *, uint16_t);

#endif
