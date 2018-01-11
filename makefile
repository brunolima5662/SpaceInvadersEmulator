EXEC = build/space-invaders
ROM = rom/rom
OBJECTS = build/disassembler.o build/emulator.o build/machine.o build/media.o
HEADERS = src/machine.h src/disassembler.h src/emulator.h src/media.h
FLAGS = -Wall `sdl2-config --cflags`
LIBS = `sdl2-config --libs` -lSDL2_mixer


space-invaders: src/main.c $(HEADERS) $(OBJECTS)
	cc src/main.c $(OBJECTS) -o $(EXEC) $(FLAGS) $(LIBS)

build/emulator.o: src/emulator.c src/machine.h
	cc -c src/emulator.c -o build/emulator.o $(FLAGS) $(LIBS)

build/disassembler.o: src/disassembler.c
	cc -c src/disassembler.c -o build/disassembler.o $(FLAGS)

build/machine.o: src/machine.c src/media.h
	cc -c src/machine.c -o build/machine.o $(FLAGS) $(LIBS)

build/media.o: src/media.asm
	nasm -f elf64 -i "media/" src/media.asm -o build/media.o

clean:
	rm $(EXEC) $(OBJECTS)
