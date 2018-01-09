bits 64

section .rodata

global _rom_start
global _rom_end
global _rom_size

_rom_start:   incbin FILE
_rom_end:
_rom_size:    dd $-_rom_start
