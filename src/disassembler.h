
#ifndef DISASSEMBLER
#define DISASSEMBLER

#include <stdint.h>
#include <stdio.h>

uint8_t disassemble_opcode(unsigned char *, uint16_t *);
uint8_t disassemble_at_memory(unsigned char *, uint16_t);
int disassemble(unsigned char *, uint16_t);

#endif
