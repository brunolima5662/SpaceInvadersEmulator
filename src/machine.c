#include "machine.h"

void initialize_machine(machine_t * state) {
    for(uint8_t i = 0; i < IO_PORTS; i++)
        state->ports[i] = 0;
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
    state->last_processed = 0;
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

void sleep_nanoseconds(long nanoseconds) {
    struct timespec ts;
    ts.tv_sec  = 0;
    ts.tv_nsec = nanoseconds;
    nanosleep(&ts, NULL);
}
