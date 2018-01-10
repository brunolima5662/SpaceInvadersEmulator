# SpaceInvadersEmulator

My attempt at creating an emulator for Space Invaders (Intel 8080 CPU)

Building is only supported on UNIX based systems.  
On macs, change the -f flag in the makefile's rom.o rule from elf64 to macho64.

### Dependencies

* [SDL 2.0](https://www.libsdl.org/index.php)
* [NASM Assembly Compiler](http://www.nasm.us/)

### Build & Install

1. `make`
2. `sudo make install`

### Keyboard Controls

* Quit Emulation: *Escape*
* Full Screen Toggle: *F*
* Player 1:
    * Start: *1*
    * Shoot: *Space*
    * Move Left: *A*
    * Move Right: *D*
* Player 2:
    * Start: *2*
    * Shoot: *Keypad Enter*
    * Move Left: *Left Arrow*
    * Move Right: *Right Arrow*
