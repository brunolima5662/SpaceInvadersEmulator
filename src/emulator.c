#include "emulator.h"

uint8_t clock_cycles[] = {
/*  00  01  02  03  04  05  06  07  08  09  0a  0b  0c  0d  0e  0f	*/
	04, 10, 07, 05, 05, 05, 07, 04, 04, 10, 07, 05, 05, 05, 07, 04, /* 0x00 - 0x0f */
	04, 10, 07, 05, 05, 05, 07, 04, 04, 10, 07, 05, 05, 05, 07, 04, /* 0x10 - 0x1f */
	04, 10, 16, 05, 05, 05, 07, 04, 04, 10, 16, 05, 05, 05, 07, 04, /* 0x20 - 0x2f */
	04, 10, 13, 05, 10, 10, 07, 04, 04, 10, 13, 05, 05, 05, 07, 04, /* 0x30 - 0x3f */
	05, 05, 05, 05, 05, 05, 07, 05, 05, 05, 05, 05, 05, 05, 07, 05, /* 0x40 - 0x4f */
	05, 05, 05, 05, 05, 05, 07, 05, 05, 05, 05, 05, 05, 05, 07, 05, /* 0x50 - 0x5f */
	05, 05, 05, 05, 05, 05, 07, 05, 05, 05, 05, 05, 05, 05, 07, 05, /* 0x60 - 0x6f */
	07, 07, 07, 07, 07, 07, 07, 07, 05, 05, 05, 05, 05, 05, 07, 05, /* 0x70 - 0x7f */
	04, 04, 04, 04, 04, 04, 07, 04, 04, 04, 04, 04, 04, 04, 07, 04, /* 0x80 - 0x8f */
	04, 04, 04, 04, 04, 04, 07, 04, 04, 04, 04, 04, 04, 04, 07, 04, /* 0x90 - 0x9f */
	04, 04, 04, 04, 04, 04, 07, 04, 04, 04, 04, 04, 04, 04, 07, 04, /* 0xa0 - 0xaf */
	04, 04, 04, 04, 04, 04, 07, 04, 04, 04, 04, 04, 04, 04, 07, 04, /* 0xb0 - 0xbf */
	11, 10, 10, 10, 17, 11, 07, 11, 11, 10, 10, 04, 17, 17, 07, 11, /* 0xc0 - 0xcf */
	11, 10, 10, 10, 17, 11, 07, 11, 11, 04, 10, 10, 17, 04, 07, 11, /* 0xd0 - 0xdf */
	11, 10, 10, 18, 17, 11, 07, 11, 11, 05, 10, 04, 17, 04, 07, 11, /* 0xe0 - 0xef */
	11, 10, 10, 04, 17, 11, 07, 11, 11, 05, 10, 04, 17, 04, 07, 11  /* 0xf0 - 0xff */
};

void update_flags(machine_t * state, uint16_t value, uint8_t mask) {
    if(mask & 0x01)
        state->z  = (value == 0) ? 1 : 0;
    if(mask & 0x02)
        state->s  = (uint8_t)((value >> 7) & 0x01);
    if(mask & 0x04)
        state->p  = (uint8_t)(~value & 0x01);
    if(mask & 0x08)
        state->cy = (uint8_t)((value >> 8) & 0x01);
}

uint8_t emulate_next_instruction(machine_t * state) {
	uint8_t  tmp;
    uint16_t result;
    unsigned char * op = &state->memory[state->pc];
    switch(*op) {
        case 0x00: break;
        case 0x01: state->b = op[2]; state->c = op[1]; state->pc += 2; break;
        case 0x02: state->memory[(state->b << 8) | state->c] = state->a; break;
        case 0x03:
            result   = ((state->b << 8) | state->c) + 1;
            state->b = (uint8_t)(result >> 8);
            state->c = (uint8_t)(result & 0xff);
            break;
        case 0x04:
            result = (uint16_t)state->b + 1;
            state->b = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x07);
            break;
        case 0x05:
            result = (uint16_t)state->b - 1;
            state->b = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x07);
            break;
        case 0x06: state->b = op[1]; state->pc += 1; break;
        case 0x07:
            state->cy = ((state->a >> 7) & 0x01);
            state->a  = (state->a << 1) | ((state->a >> 7) & 0x01);
            break;
        case 0x08: break;
        case 0x09:
            result = ((state->h << 8) | state->l) + ((state->b << 8) | state->c);
            state->h = (uint8_t)(result >> 8);
            state->l = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x08);
            break;
        case 0x0a: state->a = state->memory[(state->b << 8) | state->c]; break;
        case 0x0b:
            result = ((state->b << 8) | state->c) - 1;
            state->b = (uint8_t)(result >> 8);
            state->c = (uint8_t)(result & 0xff);
            break;
        case 0x0c:
            result = (uint16_t)state->c + 1;
        	state->c = (uint8_t)(result & 0xff);
            update_flags(state, (uint16_t)state->c, 0x07);
            break;
        case 0x0d:
            result = (uint16_t)state->c + 1;
        	state->c = (uint8_t)(result & 0xff);
            update_flags(state, (uint16_t)state->c, 0x07);
            break;
        case 0x0e: state->c = op[1]; state->pc += 1; break;
        case 0x0f:
            state->cy = (state->a & 0x01);
            state->a  = (state->a >> 1) | ((state->a << 7) & 0x80);
            break;
        case 0x10: break;
        case 0x11: state->d = op[2]; state->e = op[1]; state->pc += 2; break;
        case 0x12: state->memory[(state->d << 8) | state->e] = state->a; break;
        case 0x13:
            result   = ((state->d << 8) | state->e) + 1;
            state->d = (uint8_t)(result >> 8);
            state->e = (uint8_t)(result & 0xff);
            break;
        case 0x14:
            result = (uint16_t) state->d + 1;
        	state->d =  (uint8_t)(result & 0xff);
            update_flags(state, (uint16_t)state->d, 0x07);
            break;
        case 0x15:
            result = (uint16_t)state->d - 1;
        	state->d =  (uint8_t)(result & 0xff);
            update_flags(state, (uint16_t)state->d, 0x07);
            break;
        case 0x16: state->d = op[1]; state->pc += 1; break;
        case 0x17:
        	tmp = state->cy;
        	state->cy = ((state->a  >> 7) & 0x01);
            state->a  = (state->a << 1) | tmp;
            break;
        case 0x18: break;
        case 0x19:
            result = ((state->h << 8) | state->l) + ((state->d << 8) | state->e);
            state->h = (uint8_t)(result >> 8);
            state->l = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x08);
            break;
        case 0x1a: state->a = state->memory[(state->d << 8) | state->e]; break;
        case 0x1b:
        	result = ((state->d << 8) | state->e) - 1;
        	state->d = (uint8_t)(result >> 8);
        	state->e = (uint8_t)(result & 0xff);
			break;
        case 0x1c:
        	result = (uint16_t)state->e + 1;
        	state->e = (uint8_t)(result & 0xff);
        	update_flags(state, result, 0x07);
			break;
        case 0x1d:
        	result = (uint16_t)state->e - 1;
        	state->e = (uint8_t)(result & 0xff);
        	update_flags(state, result, 0x07);
        break;
        case 0x1e: state->e = op[1]; state->pc += 1; break;
        case 0x1f:
        	state->cy = state->a & 0x01;
        	state->a = (state->a & 0x80) | (state->a >> 1);
        	break;
        case 0x20: break;
        case 0x21: state->h = op[2]; state->l = op[1]; state->pc += 2; break;
        case 0x22:
        	result = (op[2] << 8) | (uint16_t)op[1];
        	state->memory[result] = state-> l;
        	state->memory[result + 1] = state->h;
			state->pc += 2;
			break;
        case 0x23:
            result   = ((state->h << 8) | state->l) + 1;
            state->h = (uint8_t)(result >> 8);
            state->l = (uint8_t)(result & 0xff);
            break;
        case 0x24:
            result = (uint16_t)state->h + 1;
            state->h = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x07);
            break;
        case 0x25:
            result = (uint16_t)state->h - 1;
            state->h = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x07);
            break;
        case 0x26: state->h = op[1]; state->pc += 1; break;
        case 0x27:
            result = (uint16_t)state->a;
            if((result & 0x000f) > 9)
                result += 0x06;
            if(((result & 0x00f0) >> 4) > 9 || state->cy)
                result += 0x60;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x28: break;
        case 0x29:
            result = ((state->h << 8) | state->l) + ((state->h << 8) | state->l);
            state->h = (uint8_t)(result >> 8);
            state->l = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x08);
            break;
        case 0x2a:
            result = (op[2] << 8) | (uint16_t)op[1];
            state->l = state->memory[result];
            state->h = state->memory[result + 1];
            state->pc += 2;
            break;
        case 0x2b:
            result   = ((state->h << 8) | (uint16_t)state->l) - 1;
            state->h = (uint8_t)(result >> 8);
            state->l = (uint8_t)(result & 0xff);
            break;
        case 0x2c:
            result = (uint16_t)state->l + 1;
            state->l = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x07);
            break;
        case 0x2d:
            result = (uint16_t)state->l - 1;
            state->l = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x07);
            break;
        case 0x2e: state->l = op[1]; state->pc += 1; break;
        case 0x2f: state->a = ~state->a; break;
        case 0x30: break;
        case 0x31: state->sp = (op[2] << 8) | op[1]; state->pc += 2; break;
        case 0x32:
            result = (op[2] << 8) | op[1];
            state->memory[result] = state->a;
            break;
        case 0x33: state->sp += 1; break;
        case 0x34:
            result = (uint16_t)state->memory[(state->h << 8) | (uint16_t)state->l] + 1;
            state->memory[(state->h << 8) | (uint16_t)state->l] = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x07);
            break;
        case 0x35:
            result = (uint16_t)state->memory[(state->h << 8) | (uint16_t)state->l] - 1;
            state->memory[(state->h << 8) | (uint16_t)state->l] = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x07);
            break;
        case 0x36:
            state->memory[(state->h << 8) | (uint16_t)state->l] = op[1];
            state->pc += 1;
            break;
        case 0x37: state->cy = 1; break;
        case 0x38: break;
        case 0x39:
            result = ((state->h << 8) | state->l) + state->sp;
            state->h = (uint8_t)(result >> 8);
            state->l = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x08);
            break;
        case 0x3a:
            state->a = state->memory[(op[2] << 8) | op[1]];
            state->pc += 2;
            break;
        case 0x3b: state->sp -= 1; break;
        case 0x3c:
            result = (uint16_t)state->a + 1;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x07); break;
        case 0x3d:
            result = (uint16_t)state->a - 1;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x07); break;
        case 0x3e: state->a = op[1]; state->pc += 1; break;
        case 0x3f: state->cy = ~state->cy; break;
        case 0x40: state->b = state->b; break;
        case 0x41: state->b = state->c; break;
        case 0x42: state->b = state->d; break;
        case 0x43: state->b = state->e; break;
        case 0x44: state->b = state->h; break;
        case 0x45: state->b = state->l; break;
        case 0x46: state->b = state->memory[(state->h << 8) | state->l]; break;
        case 0x47: state->b = state->a; break;
        case 0x48: state->c = state->b; break;
        case 0x49: state->c = state->c; break;
        case 0x4a: state->c = state->d; break;
        case 0x4b: state->c = state->e; break;
        case 0x4c: state->c = state->h; break;
        case 0x4d: state->c = state->l; break;
        case 0x4e: state->c = state->memory[(state->h << 8) | state->l]; break;
        case 0x4f: state->c = state->a; break;
        case 0x50: state->d = state->b; break;
        case 0x51: state->d = state->c; break;
        case 0x52: state->d = state->d; break;
        case 0x53: state->d = state->e; break;
        case 0x54: state->d = state->h; break;
        case 0x55: state->d = state->l; break;
        case 0x56: state->d = state->memory[(state->h << 8) | state->l]; break;
        case 0x57: state->d = state->a; break;
        case 0x58: state->e = state->b; break;
        case 0x59: state->e = state->c; break;
        case 0x5a: state->e = state->d; break;
        case 0x5b: state->e = state->e; break;
        case 0x5c: state->e = state->h; break;
        case 0x5d: state->e = state->l; break;
        case 0x5e: state->e = state->memory[(state->h << 8) | state->l]; break;
        case 0x5f: state->e = state->a; break;
        case 0x60: state->h = state->b; break;
        case 0x61: state->h = state->c; break;
        case 0x62: state->h = state->d; break;
        case 0x63: state->h = state->e; break;
        case 0x64: state->h = state->h; break;
        case 0x65: state->h = state->l; break;
        case 0x66: state->h = state->memory[(state->h << 8) | state->l]; break;
        case 0x67: state->h = state->a; break;
        case 0x68: state->l = state->b; break;
        case 0x69: state->l = state->c; break;
        case 0x6a: state->l = state->d; break;
        case 0x6b: state->l = state->e; break;
        case 0x6c: state->l = state->h; break;
        case 0x6d: state->l = state->l; break;
        case 0x6e: state->l = state->memory[(state->h << 8) | state->l]; break;
        case 0x6f: state->l = state->a; break;
        case 0x70: state->memory[(state->h << 8) | state->l] = state->b; break;
        case 0x71: state->memory[(state->h << 8) | state->l] = state->c; break;
        case 0x72: state->memory[(state->h << 8) | state->l] = state->d; break;
        case 0x73: state->memory[(state->h << 8) | state->l] = state->e; break;
        case 0x74: state->memory[(state->h << 8) | state->l] = state->h; break;
        case 0x75: state->memory[(state->h << 8) | state->l] = state->l; break;
        case 0x76: return 0; break; // halt command, stop emulation...
        case 0x77: state->memory[(state->h << 8) | state->l] = state->a; break;
        case 0x78: state->a = state->b; break;
        case 0x79: state->a = state->c; break;
        case 0x7a: state->a = state->d; break;
        case 0x7b: state->a = state->e; break;
        case 0x7c: state->a = state->h; break;
        case 0x7d: state->a = state->l; break;
        case 0x7e: state->a = state->memory[(state->h << 8) | state->l]; break;
        case 0x7f: state->a = state->a; break;
        case 0x80:
            result = state->a + state->b;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x81:
            result = state->a + state->c;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x82:
            result = state->a + state->d;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x83:
            result = state->a + state->e;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x84:
            result = state->a + state->h;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x85:
            result = state->a + state->l;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x86:
            result = state->a + state->memory[(state->h << 8) | state->l];
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x87:
            result = state->a + state->a;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x88:
            result = state->a + state->b + state->cy;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x89:
            result = state->a + state->c + state->cy;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x8a:
            result = state->a + state->d + state->cy;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x8b:
            result = state->a + state->e + state->cy;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x8c:
            result = state->a + state->h + state->cy;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x8d:
            result = state->a + state->l + state->cy;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x8e:
            result = state->a + state->memory[(state->h << 8) | state->l] + state->cy;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x8f:
            result = state->a + state->a + state->cy;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x90:
            result = state->a - state->b;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x91:
            result = state->a - state->c;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x92:
            result = state->a - state->d;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x93:
            result = state->a - state->e;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x94:
            result = state->a - state->h;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x95:
            result = state->a - state->l;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x96:
            result = state->a - state->memory[(state->h << 8) | state->l];
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x97:
            result = state->a - state->a;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x98:
            result = state->a - state->b - state->cy;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x99:
            result = state->a - state->c - state->cy;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x9a:
            result = state->a - state->d - state->cy;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x9b:
            result = state->a - state->e - state->cy;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x9c:
            result = state->a - state->h - state->cy;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x9d:
            result = state->a - state->l - state->cy;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x9e:
            result = state->a - state->memory[(state->h << 8) | state->l] - state->cy;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0x9f:
            result = state->a - state->a - state->cy;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0xa0:
            result = state->a & state->b;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0xa1:
            result = state->a & state->c;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0xa2:
            result = state->a & state->d;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0xa3:
            result = state->a & state->e;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0xa4:
            result = state->a & state->h;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0xa5:
            result = state->a & state->l;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0xa6:
            result = state->a & state->memory[(state->h << 8) | state->l];
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0xa7:
            result = state->a & state->a;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0xa8:
            result = state->a ^ state->b;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0xa9:
            result = state->a ^ state->c;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0xaa:
            result = state->a ^ state->d;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0xab:
            result = state->a ^ state->e;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0xac:
            result = state->a ^ state->h;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0xad:
            result = state->a ^ state->l;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0xae:
            result = state->a ^ state->memory[(state->h << 8) | state->l];
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0xaf:
            result = state->a ^ state->a;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0xb0:
            result = state->a | state->b;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0xb1:
            result = state->a | state->c;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0xb2:
            result = state->a | state->d;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0xb3:
            result = state->a | state->e;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0xb4:
            result = state->a | state->h;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0xb5:
            result = state->a | state->l;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0xb6:
            result = state->a | state->memory[(state->h << 8) | state->l];
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0xb7:
            result = state->a | state->a;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            break;
        case 0xb8:
            tmp = state->a - state->b;
            state->z  = (tmp == 0) ? 1 : 0;
            state->s  = (tmp >> 7) & 0x01;
            state->p  = (~tmp & 0x01);
            state->cy = (state->a < state->b) ? 1 : 0;
            break;
        case 0xb9:
            tmp = state->a - state->c;
            state->z  = (tmp == 0) ? 1 : 0;
            state->s  = (tmp >> 7) & 0x01;
            state->p  = (~tmp & 0x01);
            state->cy = (state->a < state->c) ? 1 : 0;
            break;
        case 0xba:
            tmp = state->a - state->d;
            state->z  = (tmp == 0) ? 1 : 0;
            state->s  = (tmp >> 7) & 0x01;
            state->p  = (~tmp & 0x01);
            state->cy = (state->a < state->d) ? 1 : 0;
            break;
        case 0xbb:
            tmp = state->a - state->e;
            state->z  = (tmp == 0) ? 1 : 0;
            state->s  = (tmp >> 7) & 0x01;
            state->p  = (~tmp & 0x01);
            state->cy = (state->a < state->e) ? 1 : 0;
            break;
        case 0xbc:
            tmp = state->a - state->h;
            state->z  = (tmp == 0) ? 1 : 0;
            state->s  = (tmp >> 7) & 0x01;
            state->p  = (~tmp & 0x01);
            state->cy = (state->a < state->h) ? 1 : 0;
            break;
        case 0xbd:
            tmp = state->a - state->l;
            state->z  = (tmp == 0) ? 1 : 0;
            state->s  = (tmp >> 7) & 0x01;
            state->p  = (~tmp & 0x01);
            state->cy = (state->a < state->l) ? 1 : 0;
            break;
        case 0xbe:
            tmp = state->a - state->memory[(state->h << 8) | state->l];
            state->z  = (tmp == 0) ? 1 : 0;
            state->s  = (tmp >> 7) & 0x01;
            state->p  = (~tmp & 0x01);
            state->cy = (state->a < state->memory[(state->h << 8) | state->l]) ? 1 : 0;
            break;
        case 0xbf:
            tmp = state->a - state->a;
            state->z  = (tmp == 0) ? 1 : 0;
            state->s  = (tmp >> 7) & 0x01;
            state->p  = (~tmp & 0x01);
            state->cy = (state->a < state->a) ? 1 : 0;
            break;
        case 0xc0:
        	if(state->z == 0) {
        		state->pc = (state->memory[state->sp + 1] << 8) | (uint8_t)state->memory[state->sp];
        		state->sp += 2;
        	}
			break;
        case 0xc1:
        	state->c = state->memory[state->sp];
        	state->b = state->memory[state->sp + 1];
        	state->sp = state->sp + 2;
			break;
        case 0xc2:
        	if(state->z == 0)
            	state->pc = (op[2] << 8) | op[1];
            else
                state->pc += 2;
			break;
        case 0xc3: state->pc = (op[2] << 8) | op[1]; break;
        case 0xc4:
            if(state->z == 0) {
                result = state->pc + 2;
            	state->memory[state->sp - 1] = (uint8_t)(result >> 8);
            	state->memory[state->sp - 2] = (uint8_t)(result & 0xff);
            	state->sp -= 2;
            	state->pc = (op[2] << 8) | op[1];
            }
            else {
                state->pc += 2;
            }
			break;
        case 0xc5:
            state->memory[state->sp - 2] = state->c;
            state->memory[state->sp - 1] = state->b;
            state->sp -= 2;
            break;
        case 0xc6:
            result = state->a + op[1];
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            state->pc += 1; break;
        case 0xc7:
            state->memory[state->sp - 1] = (uint8_t)(state->pc >> 8);
            state->memory[state->sp - 2] = (uint8_t)(state->pc & 0xff);
            state->sp -= 2;
            state->pc = 0x00;
            break;
        case 0xc8:
            if(state->z != 0) {
                state->pc = (state->memory[state->sp + 1] << 8) | (uint8_t)state->memory[state->sp];
                state->sp += 2;
            }
            break;
        case 0xc9:
            state->pc = (state->memory[state->sp + 1] << 8) | state->memory[state->sp];
            state->sp += 2;
            break;
        case 0xca:
            if(state->z)
                state->pc = (op[2] << 8) | op[1];
            else
                state->pc += 2;
            break;
        case 0xcb: break;
        case 0xcc:
            if(state->z) {
                result = state->pc + 2;
                state->memory[state->sp - 1] = (uint8_t)(result >> 8);
                state->memory[state->sp - 2] = (uint8_t)(result & 0xff);
                state->sp -= 2;
                state->pc = (op[2] << 8) | op[1];
            }
            else {
                state->pc += 2;
            }
            break;
        case 0xcd:
            result = state->pc + 2;
            state->memory[state->sp - 1] = (uint8_t)(result >> 8);
            state->memory[state->sp - 2] = (uint8_t)(result & 0xff);
            state->sp += 2;
            state->pc = (op[2] << 8) | op[1];
            break;
        case 0xce:
            result = (uint16_t)state->a + (uint16_t)op[1] + (uint16_t)state->cy;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            state->pc += 1; break;
        case 0xcf:
            state->memory[state->sp - 1] = (uint8_t)(state->pc >> 8);
            state->memory[state->sp - 2] = (uint8_t)(state->pc & 0xff);
            state->sp -= 2;
            state->pc = 0x08;
            break;
        case 0xd0:
            if(state->cy == 0) {
                state->pc = (state->memory[state->sp + 1] << 8) | (uint8_t)state->memory[state->sp];
                state->sp += 2;
            }
            break;
        case 0xd1:
            state->d = state->memory[state->sp + 1];
            state->e = state->memory[state->sp];
            state->sp += 2;
            break;
        case 0xd2:
            if(state->cy == 0)
                state->pc = (op[2] << 8) | op[1];
            else
                state->pc += 2;
            break;
        case 0xd3: state->ports[op[1]] = state->a; state->pc += 1; break;
        case 0xd4:
            if(state->cy == 0) {
                result = state->pc + 2;
                state->memory[state->sp - 1] = (uint8_t)(result >> 8);
                state->memory[state->sp - 2] = (uint8_t)(result & 0xff);
                state->sp -= 2;
                state->pc = (op[2] << 8) | op[1];
            }
            else {
                state->pc += 2;
            }
            break;
        case 0xd5:
            state->memory[state->sp - 2] = state->e;
            state->memory[state->sp - 1] = state->d;
            state->sp -= 2;
            break;
        case 0xd6:
            result   = (uint16_t)state->a - (uint16_t)op[1];
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            state->pc += 1; break;
        case 0xd7:
            state->memory[state->sp - 1] = (uint8_t)(state->pc >> 8);
            state->memory[state->sp - 2] = (uint8_t)(state->pc & 0xff);
            state->sp -= 2;
            state->pc = 0x10;
            break;
        case 0xd8:
            if(state->cy != 0) {
                state->pc = (state->memory[state->sp + 1] << 8) | (uint8_t)state->memory[state->sp];
                state->sp += 2;
            }
            break;
        case 0xd9: break;
        case 0xda:
            if(state->cy != 0)
                state->pc = (op[2] << 8) | op[1];
            else
                state->pc += 2;
            break;
        case 0xdb: state->a = state->ports[op[1]]; state->pc += 1; break;
        case 0xdc:
            if(state->cy != 0) {
                result = state->pc + 2;
                state->memory[state->sp - 1] = (uint8_t)(result >> 8);
                state->memory[state->sp - 2] = (uint8_t)(result & 0xff);
                state->sp -= 2;
                state->pc = (op[2] << 8) | op[1];
            }
            else {
                state->pc += 2;
            }
            break;
        case 0xdd: break;
        case 0xde:
            result = (uint16_t)state->a - (uint16_t)op[1] - (uint16_t)state->cy;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            state->pc += 1; break;
        case 0xdf:
            state->memory[state->sp - 1] = (uint8_t)(state->pc >> 8);
            state->memory[state->sp - 2] = (uint8_t)(state->pc & 0xff);
            state->sp -= 2;
            state->pc = 0x18;
            break;
        case 0xe0:
            if(state->p == 0) {
                state->pc = (state->memory[state->sp + 1] << 8) | (uint8_t)state->memory[state->sp];
                state->sp += 2;
            }
            break;
        case 0xe1:
            state->l = state->memory[state->sp];
            state->h = state->memory[state->sp + 1];
            state->sp += 2;
            break;
        case 0xe2:
            if(state->p == 0)
                state->pc = (op[2] << 8) | op[1];
            else
                state->pc += 2;
            break;
        case 0xe3:
            result = (state->memory[state->sp] << 8) || state->memory[state->sp + 1];
            state->memory[state->sp] = state->l;
            state->memory[state->sp + 1] = state->h;
            state->l = (uint8_t)(result >> 8);
            state->h = (uint8_t)(result & 0xff);
            break;
        case 0xe4:
            if(state->p == 0) {
                result = state->pc + 2;
                state->memory[state->sp - 1] = (uint8_t)(result >> 8);
                state->memory[state->sp - 2] = (uint8_t)(result & 0xff);
                state->sp -= 2;
                state->pc = (op[2] << 8) | op[1];
            }
            else {
                state->pc += 2;
            }
            break;
        case 0xe5:
            state->memory[state->sp - 2] = state->l;
            state->memory[state->sp - 1] = state->h;
            state->sp -= 2;
            break;
        case 0xe6:
            result = state->a & op[1];
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            state->pc += 1;
            break;
        case 0xe7:
            state->memory[state->sp - 1] = (uint8_t)(state->pc >> 8);
            state->memory[state->sp - 2] = (uint8_t)(state->pc & 0xff);
            state->sp -= 2;
            state->pc = 0x20;
            break;
        case 0xe8:
            if(state->p == 1) {
                state->pc = (state->memory[state->sp + 1] << 8) | (uint8_t)state->memory[state->sp];
                state->sp += 2;
            }
            break;
        case 0xe9:
            state->pc = (state->h << 8) | state->l;
            break;
        case 0xea:
            if(state->p == 1)
                state->pc = (op[2] << 8) | op[1];
            else
                state->pc += 2;
            break;
        case 0xeb:
            result = (state->h << 8) || state->l;
            state->h = state->d;
            state->l = state->e;
            state->d = (uint8_t)(result >> 8);
            state->e = (uint8_t)(result & 0xff);
            break;
        case 0xec:
            if(state->p == 1) {
                result = state->pc + 2;
                state->memory[state->sp - 1] = (uint8_t)(result >> 8);
                state->memory[state->sp - 2] = (uint8_t)(result & 0xff);
                state->sp -= 2;
                state->pc = (op[2] << 8) | op[1];
            }
            else {
                state->pc += 2;
            }
            break;
        case 0xed: break;
        case 0xee:
            result = state->a ^ op[1];
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            state->pc += 1; break;
        case 0xef:
            state->memory[state->sp - 1] = (uint8_t)(state->pc >> 8);
            state->memory[state->sp - 2] = (uint8_t)(state->pc & 0xff);
            state->sp -= 2;
            state->pc = 0x28;
            break;
        case 0xf0:
            if(state->s == 0) {
                state->pc = (state->memory[state->sp + 1] << 8) | (uint8_t)state->memory[state->sp];
                state->sp += 2;
            }
            break;
        case 0xf1:
            tmp        = state->memory[state->sp];
            state->a   = state->memory[state->sp + 1];
            state->z   = tmp & 0x01;
            state->s   = (tmp >> 1) & 0x01;
            state->p   = (tmp >> 2) & 0x01;
            state->cy  = (tmp >> 3) & 0x01;
            state->sp += 2;
            break;
        case 0xf2:
            if(state->s == 0)
                state->pc = (op[2] << 8) | op[1];
            else
                state->pc += 2;
            break;
        case 0xf3: state->accept_interrupt = 0; break;
        case 0xf4:
            if(state->s == 0)
                state->pc = (op[2] << 8) | op[1];
            else
                state->pc += 2;
            break;
        case 0xf5:
            state->memory[state->sp - 1] = state->a;
            tmp = state->z | (state->s << 1) | (state->p << 2) | (state->cy << 3);
            state->memory[state->sp - 2] = tmp;
            state->sp -= 2;
            break;
        case 0xf6:
            result = state->a | op[1];
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x0f);
            state->pc += 1; break;
        case 0xf7:
            state->memory[state->sp - 1] = (uint8_t)(state->pc >> 8);
            state->memory[state->sp - 2] = (uint8_t)(state->pc & 0xff);
            state->sp -= 2;
            state->pc = 0x30;
            break;
        case 0xf8:
            if(state->s == 1) {
                state->pc = (state->memory[state->sp + 1] << 8) | (uint8_t)state->memory[state->sp];
                state->sp += 2;
            }
            break;
        case 0xf9:
            state->sp = (state->h << 8) | state->l;
            break;
        case 0xfa:
            if(state->s == 1)
                state->pc = (op[2] << 8) | op[1];
            else
                state->pc += 2;
            break;
        case 0xfb: state->accept_interrupt = 1; break;
        case 0xfc:
            if(state->s == 1) {
                result = state->pc + 2;
                state->memory[state->sp - 1] = (uint8_t)(result >> 8);
                state->memory[state->sp - 2] = (uint8_t)(result & 0xff);
                state->sp -= 2;
                state->pc = (op[2] << 8) | op[1];
            }
            else {
                state->pc += 2;
            }
        case 0xfd: break;
        case 0xfe:
            tmp = state->a - op[1];
            state->z  = (tmp == 0) ? 1 : 0;
            state->s  = (tmp >> 7) & 0x01;
            state->p  = (~tmp & 0x01);
            state->cy = (state->a < op[1]) ? 1 : 0;
            state->pc += 1; break;
        case 0xff:
            state->memory[state->sp - 1] = (uint8_t)(state->pc >> 8);
            state->memory[state->sp - 2] = (uint8_t)(state->pc & 0xff);
            state->sp -= 2;
            state->pc = 0x38;
            break;
        default: return 0;
    }
    return 1;
}
