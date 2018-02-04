#include "machine.h"

void interrupt_cpu(machine_t *, uint8_t);
void reset_program(machine_t *, uint8_t);
void load_sound_samples(machine_t *);

void initialize_machine(machine_t * state, uint8_t lives) {
    memset(state->memory, 0, MEMORY_SIZE);
    memset(state->channels, 0, SOUND_SAMPLES * 4);
    load_sound_samples(state);
    reset_program(state, lives);
}

void load_machine(machine_t * state, uint8_t * saved_machine, uint8_t * saved_state) {
    memcpy(state->memory, saved_state, MEMORY_SIZE);
    memset(state->channels, 0, SOUND_SAMPLES * 4);
    load_sound_samples(state);
    state->a = saved_machine[0];
    state->b = saved_machine[1];
    state->c = saved_machine[2];
    state->d = saved_machine[3];
    state->e = saved_machine[4];
    state->h = saved_machine[5];
    state->l = saved_machine[6];
    state->pc = (saved_machine[7] << 8) | saved_machine[8];
    state->sp = (saved_machine[9] << 8) | saved_machine[10];
    state->z = saved_machine[11];
    state->s = saved_machine[12];
    state->p = saved_machine[13];
    state->cy = saved_machine[14];
    state->shift_hi = saved_machine[15];
    state->shift_lo = saved_machine[16];
    state->shift_offset = saved_machine[17];
    state->accept_interrupt = saved_machine[18];
    memcpy(state->ports_in, &(saved_machine[19]), IO_PORTS);
    memcpy(state->ports_out, &(saved_machine[19 + IO_PORTS]), IO_PORTS);
    free(saved_machine);
    free(saved_state);
}

void reset_program(machine_t * state, uint8_t lives) {
    memset(state->ports_in, 0, IO_PORTS);
    memset(state->ports_out, 0, IO_PORTS);
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
    state->ports_in[0] = 0x0e; // bits 1, 2, and 3 are always set
    state->ports_in[1] = 0x08; // bit 3 is always set
    state->ports_in[2] = 0x08 | (lives - 3); // bit 3 is always set

}

void shutdown_machine(machine_t * state) {
    for(uint8_t i = 0; i < SOUND_SAMPLES; i++) {
        Mix_FreeChunk(state->samples[i]);
    }
}

int check_machine_instruction(machine_t * state) {
    int sound_res;
    uint16_t value;
    uint8_t changes, port;
    uint8_t * op = &state->memory[state->pc];
    if(*op == 0xdb) { // IN
        switch(op[1]) { // port switch
            case 0: state->a = 1; break;
            case 1:
            case 2: state->a = state->ports_in[op[1]]; break;
            case 3:
                value = (state->shift_hi << 8) | state->shift_lo;
                state->a = (uint8_t)((value >> (8 - state->shift_offset)) & 0xff);
                break;
            default: ;
        }
        state->pc += 2;
        return 1;
    }
    else if(*op == 0xd3) { // OUT
        switch(op[1]) { // port switch
            case 2:
                state->shift_offset = state->a & 0x07;
                break;
            case 3: // play sound
                // get which bits changed since last time by doing an
                // XOR with the port's cache value
                changes = state->ports_out[op[1]] ^ state->a;
                if(changes) {
                    port = state->a;
                    state->ports_out[op[1]] = state->a;
                    if(changes & 0x01) { // ufo sound
                        if(port & 0x01) { // start sound
                            sound_res = Mix_PlayChannel(-1, state->samples[0], -1);
                            state->channels[0] = sound_res;
                        }
                        else { // stop sound
                            sound_res = Mix_HaltChannel(state->channels[0]);
                            state->channels[0] = 0;
                        }
                    }
                    if(changes & port & 0x02) // shot sound
                        sound_res = Mix_PlayChannel(-1, state->samples[1], 0);
                    if(changes & port & 0x04) // player hit sound
                        sound_res = Mix_PlayChannel(-1, state->samples[2], 0);
                    if(changes & port & 0x08) // alien hit sound
                        sound_res = Mix_PlayChannel(-1, state->samples[3], 0);
                    if(sound_res == -1)
                        fprintf(stderr, "SDL_mixer Error: %s\n", Mix_GetError());
                }
                break;
            case 4:
                state->shift_lo = state->shift_hi;
                state->shift_hi = state->a;
                break;
            case 5: // play sound
                // get which bits changed since last time by doing an
                // XOR with the port's cache value
                changes = state->ports_out[op[1]] ^ state->a;
                if(changes) {
                    port = state->a;
                    state->ports_out[op[1]] = state->a;
                    if(changes & port & 0x01) // fleet sound 1
                        sound_res = Mix_PlayChannel(-1, state->samples[4], 0);
                    if(changes & port & 0x02) // fleet sound 2
                        sound_res = Mix_PlayChannel(-1, state->samples[5], 0);
                    if(changes & port & 0x04) // fleet sound 3
                        sound_res = Mix_PlayChannel(-1, state->samples[6], 0);
                    if(changes & port & 0x08) // fleet sound 4
                        sound_res = Mix_PlayChannel(-1, state->samples[7], 0);
                    if(changes & port & 0x10) // UFO hit sound
                        sound_res = Mix_PlayChannel(-1, state->samples[8], 0);
                    if(sound_res == -1)
                        fprintf(stderr, "SDL_mixer Error: %s\n", Mix_GetError());
                }
                break;
            default: ;
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

void render_screen(machine_t * state, SDL_Surface * screen, uint8_t fcolor, uint8_t bcolor) {
    uint32_t x, y, inverse_y_start;
    uint8_t on = fcolor, off = bcolor, bit;
    uint8_t * pixels = (uint8_t *)screen->pixels, * pixels_tmp;

    unsigned char * video_ram = &state->memory[VIDEO_RAM_START];
    unsigned char pixel;

    SDL_LockSurface(screen);
    for(y = 0; y < VIDEO_Y; y++) {
        inverse_y_start = VIDEO_SIZE + (y - VIDEO_Y);
        for(x = 0; x < VIDEO_X; x += 8) {
            pixel = (*video_ram);
            pixels_tmp = &pixels[inverse_y_start - (x * VIDEO_Y)];
            for(bit = 0; bit < 8; bit++) {
                (*pixels_tmp) = ((pixel >> bit) & 0x01) ? on : off;
                pixels_tmp -= VIDEO_Y;
            }
            video_ram++;
        }
    }
    SDL_UnlockSurface(screen);
}

void update_input_bit(machine_t * state, uint8_t port, uint8_t bit, uint32_t event) {
    if(event == SDL_KEYDOWN)
        state->ports_in[port] |= (1 << bit);
    else
        state->ports_in[port] &= ~(1 << bit);
}

SI_KEY_RESULT handle_input(machine_t * state, uint32_t event, uint32_t key) {
    SI_KEY_RESULT result = SI_KEY_RESULT_OK;
    switch(key) {
        case SDLK_c: // Coin
            update_input_bit(state, 1, 0, event); break;
        case SDLK_1: // Player 1 Start
            update_input_bit(state, 1, 2, event); break;
        case SDLK_2: // Player 2 Start
            update_input_bit(state, 1, 1, event); break;
        case SDLK_SPACE: // Player 1 Shoot Button
            update_input_bit(state, 1, 4, event); break;
        case SDLK_a: // Player 2 Left Button
            update_input_bit(state, 1, 5, event); break;
        case SDLK_d: // Player 2 Right Button
            update_input_bit(state, 1, 6, event); break;
        case SDLK_RETURN2: // Player 2 Shoot Button
            update_input_bit(state, 2, 4, event); break;
        case SDLK_LEFT: // Player 2 Left Button
            update_input_bit(state, 2, 5, event); break;
        case SDLK_RIGHT: // Player 2 Right Button
            update_input_bit(state, 2, 6, event); break;
        case SDLK_r:  // Reset, keep number of lives the same
            reset_program(state, ((state->ports_in[2] & 0x03) + 3));
            break;
        case SDLK_p: result = SI_KEY_RESULT_PAUSE; break; // Pause / Unpause
        case SDLK_m: result = SI_KEY_RESULT_TOGGLE_MUTE; break; // Mute Sound
        case SDLK_ESCAPE: result = SI_KEY_RESULT_EXIT; break; // Quit Emulation
        default: ;
    }
    return result;
}

void load_sound_samples(machine_t * state) {
    SDL_RWops * sample;
    #ifndef __EMSCRIPTEN__
        uint8_t * adrs[] = { &_media_0,
            &_media_1, &_media_2, &_media_3, &_media_4,
            &_media_5, &_media_6, &_media_7, &_media_8
        };
        for(uint8_t i = 0; i < SOUND_SAMPLES; i++) {
            sample = SDL_RWFromMem((void *)adrs[i], (int)(adrs[i + 1] - adrs[i]));
            state->samples[i] = Mix_LoadWAV_RW(sample, 1);
            if(state->samples[i] == NULL)
                fprintf(stderr, "SDL_mixer Error: %s\n", Mix_GetError());
        }
    #else
        char _filename[13];
        for(uint8_t i = 0; i < SOUND_SAMPLES; i++) {
            sprintf(_filename, "assets/%d.wav", i);
            sample = SDL_RWFromFile(_filename, "rb");
            state->samples[i] = Mix_LoadWAV_RW(sample, 1);
            if(state->samples[i] == NULL)
                fprintf(stderr, "SDL_mixer Error: %s\n", Mix_GetError());
        }
    #endif
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
