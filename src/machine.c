#include "machine.h"

void interrupt_cpu(machine_t *, uint8_t);

void initialize_machine(machine_t * state) {
    memset(state->memory, 0, MEMORY_SIZE);
    memset(state->ports, 0, IO_PORTS);
    state->a = 0;
    state->b = 0;
    state->c = 0;
    state->d = 0;
    state->e = 0;
    state->h = 0;
    state->l = 0;
    state->pc = 0;
    state->sp = 0;
    state->z = 0;
    state->s = 0;
    state->p = 0;
    state->cy = 0;
    state->shift_hi = 0;
    state->shift_lo = 0;
    state->shift_offset = 0;
    state->accept_interrupt = 0;
    state->ports[0] = 0x0e; // bits 1, 2, and 3 are always set
    state->ports[1] = 0x08; // bit 3 is always set
    state->ports[2] = 0x08 | (GAME_NUMBER_OF_LIVES - 3); // bit 3 is always set
}

void load_rom(machine_t * state, uint16_t start) {
    memcpy(&state->memory[start], &_rom_start, _rom_size);
}

int check_machine_instruction(machine_t * state) {
    uint16_t value;
    uint8_t * op = &state->memory[state->pc];
    if(*op == 0xdb) { // IN
        switch(op[1]) { // port switch
            case 3:
                value = (state->shift_hi << 8) | state->shift_lo;
                state->a = (uint8_t)((value >> (8 - state->shift_offset)) & 0xff);
                break;
            default:
                state->a = state->ports[op[1]];
        }
        state->pc += 2;
        return 1;
    }
    else if(*op == 0xd3) { // OUT
        switch(op[1]) { // port switch
            case 2:
                state->shift_offset = state->a & 0x07;
                break;
            case 4:
                state->shift_lo = state->shift_hi;
                state->shift_hi = state->a;
                break;
            default:
                state->ports[op[1]] = state->a;
        }
        state->pc += 2;
        return 1;
    }
    return 0;
}

void interrupt_cpu(machine_t * state, uint8_t interrupt) {
    state->memory[state->sp - 1] = (uint8_t)(state->pc >> 8);
    state->memory[state->sp - 2] = (uint8_t)(state->pc & 0xff);
    state->sp -= 2;
    state->pc = interrupt * 0x08;
    state->accept_interrupt = 0;
}

void render_scaled_pixel(uint32_t * pixels, uint32_t scaled_w, uint32_t color) {
    uint32_t x, y, y_limit = (VIDEO_SCALE * scaled_w);
    for(y = 0; y < y_limit; y += scaled_w) {
        for(x = 0; x < VIDEO_SCALE; x++) {
            pixels[x + y] = color;
        }
    }
}

void render_frame(machine_t * state, SDL_Surface * frame) {
    unsigned char * video_ram = &state->memory[VIDEO_RAM_START];
    uint32_t white = SDL_MapRGB(frame->format, 0xff, 0xff, 0xff);
    uint32_t black = SDL_MapRGB(frame->format, 0x00, 0x00, 0x00);
    uint32_t color;
    uint32_t scaled_w = VIDEO_Y * VIDEO_SCALE;
    uint32_t scaled_h = VIDEO_X * VIDEO_SCALE;
    uint32_t scaled_size = scaled_w * scaled_h;


    SDL_LockSurface(frame);
    uint32_t * pixels = (uint32_t *)frame->pixels;
    uint16_t x, y, bit;
    uint32_t offset;
    unsigned char pixel;
    for(y = 0; y < VIDEO_Y; y++) {
        for(x = 0; x < VIDEO_SCANLINE; x++) {
            pixel   = video_ram[(y * VIDEO_SCANLINE) + x];
            offset  = scaled_size - ((x * VIDEO_SCALE * 8) * scaled_w);
            offset += (y * VIDEO_SCALE);
            offset -= (scaled_w * VIDEO_SCALE);
            for(bit = 0; bit < 8; bit++) {
                color = ((pixel >> bit) & 0x01) ? white : black;
                render_scaled_pixel(&pixels[offset], scaled_w, color);
                offset -= (scaled_w * VIDEO_SCALE);
            }
        }
    }
    SDL_UnlockSurface(frame);
}

void update_input_bit(machine_t * state, uint8_t port, uint8_t bit, uint32_t event) {
    if(event == SDL_KEYDOWN)
        state->ports[port] |= (1 << bit);
    else
        state->ports[port] &= ~(1 << bit);
}

uint8_t handle_input(machine_t * state, uint32_t event, uint32_t key) {
    uint8_t result = 0;
    switch(key) {
        case SDL_SCANCODE_1: // Player 1 Start
            update_input_bit(state, 1, 2, event); break;
        case SDL_SCANCODE_2: // Player 2 Start
            update_input_bit(state, 1, 1, event); break;
        case SDL_SCANCODE_SPACE: // Player 1 Shoot Button
            update_input_bit(state, 1, 4, event); break;
        case SDL_SCANCODE_A: // Player 2 Left Button
            update_input_bit(state, 1, 5, event); break;
        case SDL_SCANCODE_D: // Player 2 Right Button
            update_input_bit(state, 1, 6, event); break;
        case SDL_SCANCODE_RETURN2: // Player 2 Shoot Button
            update_input_bit(state, 2, 4, event); break;
        case SDL_SCANCODE_LEFT: // Player 2 Left Button
            update_input_bit(state, 2, 5, event); break;
        case SDL_SCANCODE_RIGHT: // Player 2 Right Button
            update_input_bit(state, 2, 6, event); break;
        case SDL_SCANCODE_F: result = 2; break; // Fullscreen Toggle
        case SDL_SCANCODE_ESCAPE: result = 1; break; // Quit Emulation
        default: ;
    }
    return result;
}

void sleep_microseconds(uint64_t microseconds) {
    struct timespec ts;
    int result;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_nsec += (microseconds * 1000);
    if(ts.tv_nsec > 999999999UL) {
        ts.tv_sec += 1;
        ts.tv_nsec -= 1000000000UL;
    }
    do {
        result = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &ts, NULL);
    } while(result != 0);
}
