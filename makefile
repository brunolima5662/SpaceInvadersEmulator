space-invaders: src/main.c src/disassembler.c src/emulator.c src/machine.c
	gcc src/main.c src/disassembler.c src/emulator.c src/machine.c -o build/space-invaders  -Wall -lSDL2 -lpthread
