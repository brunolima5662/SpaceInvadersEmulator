CC=emcc
BUILD_DIR=build
OUTPUT=$(BUILD_DIR)/bin.js
FLAGS= --use-preload-plugins -s USE_SDL=2

space-invaders: main.c
	$(CC) main.c -s WASM=1 $(FLAGS) -o $(OUTPUT)

clean:
	rm build/*.wasm $(OUTPUT)
