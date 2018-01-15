# SpaceInvadersEmulator

My attempt at creating an emulator for Space Invaders (Intel 8080 CPU).

**_Building is only supported on UNIX based systems._**

### Dependencies

* [SDL 2.0](https://www.libsdl.org/index.php)
* [SDL_mixer 2.0](https://www.libsdl.org/projects/SDL_mixer/)
* [NASM Assembly Compiler](http://www.nasm.us/)

### Build & Install

1. `make`
2. `sudo make install`

### Uninstall

`sudo make uninstall`

### Run

`space-invaders <path to rom file...>`

### Keyboard Controls

* Pause: **P**
* Reset: **R**
* Insert Coin: **C**
* Player 1:
    * Start: **1**
    * Shoot: **Space**
    * Move Left: **A**
    * Move Right: **D**
* Player 2:
    * Start: **2**
    * Shoot: **Keypad Enter**
    * Move Left: **Left Keypad Arrow**
    * Move Right: **Right Keypad Arrow**
* Toggle Mute: **M**
* Quit Emulation: **Escape**
