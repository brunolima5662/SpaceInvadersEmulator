CC = emcc
EXEC = bin
ROM_PATH = assets/ROM
BUILD_DIR = build
OUTPUT = $(BUILD_DIR)/$(EXEC).js
FUNCTIONS = -s EXPORTED_FUNCTIONS='["_mainf", "_halt", "_toggle_pause"]' -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall"]'
ARGS = -s WASM=1  -s USE_SDL=2 -s NO_EXIT_RUNTIME=1 -s "BINARYEN_TRAP_MODE='clamp'"
FLAGS = -O2 --use-preload-plugins --preload-file assets
HEADERS = src/native/disassembler.h src/native/emulator.h src/native/machine.h
SRC = src/native/disassembler.c src/native/emulator.c src/native/machine.c

ifndef ROM
$(error Usage: make ROM=<path/to/rom/file>)
endif

all: clean copy-rom space-invaders remove-rom

copy-rom:
	cp $(ROM) $(ROM_PATH)

remove-rom:
	rm -f $(ROM_PATH)

space-invaders: src/main.c $(HEADERS) $(SRC)
	$(CC) src/main.c $(SRC) $(FLAGS) $(ARGS) $(FUNCTIONS) -o $(OUTPUT)

clean:
	rm -f $(BUILD_DIR)/$(EXEC).*
