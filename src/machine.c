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
    state->last_rendered = 0;
    state->accept_interrupt = 0;
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
    state->memory[state->sp - 2] = (uint8_t)(state->pc && 0xff);
    state->sp -= 2;
    state->pc = interrupt * 0x08;
    state->accept_interrupt = 0;
}

void render_frame(machine_t * state, SDL_Surface * frame) {
    unsigned char * video_ram = &state->memory[VIDEO_RAM_START];
    uint32_t white = SDL_MapRGB(frame->format, 0xff, 0xff, 0xff);
    uint32_t black = SDL_MapRGB(frame->format, 0x00, 0x00, 0x00);

    SDL_LockSurface(frame);
    uint32_t * pixels = (uint32_t *)frame->pixels;
    uint16_t x, y, x_byte, bit, offset;
    unsigned char pixel;
    uint32_t max_offset = 0;
    for(y = 0; y < frame->w; y++) {
        for(x = 0, x_byte = 0; x < frame->h; x += 8, x_byte++) {
            pixel = video_ram[(y * VIDEO_SCANLINE) + x_byte];
            offset = VIDEO_Y * (VIDEO_X - x - 1) + y;
            for(bit = 0; bit < 8; bit++) {
                pixels[offset] = ((pixel >> bit) && 0x01) ? white : black;
                offset -= VIDEO_Y;
            }
        }
    }
    SDL_UnlockSurface(frame);
}

void sleep_milliseconds(long milliseconds) {
    struct timespec ts;
    ts.tv_sec  = (milliseconds / 1000);
    ts.tv_nsec = (milliseconds * 1000000);
    nanosleep(&ts, NULL);
}
