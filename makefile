OS := $(shell uname -s)
ARCH := $(shell getconf LONG_BIT)
EXEC_NAME = space-invaders
EXEC = build/$(EXEC_NAME)
ROM = rom/rom
OBJECTS = build/disassembler.o build/emulator.o build/machine.o build/media.o
HEADERS = src/machine.h src/disassembler.h src/emulator.h src/media.h
FLAGS = -Wall `sdl2-config --cflags`
LIBS = `sdl2-config --libs` -lSDL2_mixer

ifeq ($(OS), Linux)
	ifeq ($(ARCH), 64)
		ASM_FORMAT = elf64
	else
		ASM_FORMAT = elf32
	endif
else ifeq ($(OS), Darwin)
	ifeq ($(ARCH), 64)
		ASM_FORMAT = macho64
	else
		ASM_FORMAT = macho32
	endif
else ifeq ($(OS), FreeBSD)
	ASM_FORMAT = aoutb
else ifeq ($(OS), NetBSD)
	ASM_FORMAT = aoutb
else
	ASM_FORMAT = aout
endif

space-invaders: src/main.c $(HEADERS) $(OBJECTS)
	cc src/main.c $(OBJECTS) -o $(EXEC) $(FLAGS) $(LIBS)

build/emulator.o: src/emulator.c src/machine.h
	cc -c src/emulator.c -o build/emulator.o $(FLAGS) $(LIBS)

build/disassembler.o: src/disassembler.c
	cc -c src/disassembler.c -o build/disassembler.o $(FLAGS)

build/machine.o: src/machine.c src/media.h src/machine.h
	cc -c src/machine.c -o build/machine.o $(FLAGS) $(LIBS)

build/media.o: src/media.asm
	nasm -f $(ASM_FORMAT) -i "media/" src/media.asm -o build/media.o

install:
	cp $(EXEC) /usr/local/bin/

uninstall:
	rm /usr/local/bin/$(EXEC_NAME)

clean:
	rm $(EXEC) $(OBJECTS)
