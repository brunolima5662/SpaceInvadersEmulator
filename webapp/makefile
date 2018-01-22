CC = emcc
BUILD_DIR = build
OUTPUT = $(BUILD_DIR)/bin.js
FUNCTIONS =  -s EXPORTED_FUNCTIONS='["_mainf"]'
ES6_MODULARIZE = '\nexport {Module as default}'
FLAGS =  -s WASM=1 -O3 --use-preload-plugins -s USE_SDL=2 --preload-file assets -s MODULARIZE=1
HEADERS = src/native/disassembler.h src/native/emulator.h src/native/machine.h
SRC = src/native/disassembler.c src/native/emulator.c src/native/machine.c

space-invaders: src/main.c $(HEADERS) $(SRC)
	$(CC) src/main.c $(SRC) $(FLAGS) $(FUNCTIONS) -o $(OUTPUT)
	echo $(ES6_MODULARIZE) | cat >> $(OUTPUT)

clean:
	rm -f build/bin.*
