CC = emcc
BUILD_DIR = build
OUTPUT = $(BUILD_DIR)/bin.js
FLAGS = -O2 --use-preload-plugins -s USE_SDL=2 --preload-file assets -s EXPORTED_FUNCTIONS='["_mainf"]'
HEADERS = src/native/disassembler.h src/native/emulator.h src/native/machine.h
SRC = src/native/disassembler.c src/native/emulator.c src/native/machine.c

space-invaders: src/main.c $(HEADERS) $(SRC)
	$(CC) src/main.c $(SRC) -s WASM=1 $(FLAGS) -o $(OUTPUT)

clean:
	rm -f build/bin.*
