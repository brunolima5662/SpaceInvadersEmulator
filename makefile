EXEC = build/space-invaders
ROM = rom/rom
OBJECTS = build/disassembler.o build/emulator.o build/machine.o
HEADERS = src/machine.h src/disassembler.h src/emulator.h
FLAGS = -Wall
LIBS = -lSDL2


space-invaders: src/main.c $(HEADERS) $(OBJECTS)
	cc src/main.c $(OBJECTS) -o $(EXEC) $(FLAGS) $(LIBS)

build/emulator.o: src/emulator.c src/machine.h
	cc -c src/emulator.c $(FLAGS) -o build/emulator.o

build/disassembler.o: src/disassembler.c
	cc -c src/disassembler.c $(FLAGS) -o build/disassembler.o

build/machine.o: src/machine.c
	cc -c src/machine.c $(FLAGS) $(LIBS) -o build/machine.o

clean:
	rm $(EXEC) $(OBJECTS)
