#include "emulator.h"

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

uint8_t emulate_instruction(machine_t * state) {
	uint8_t  tmp;
    uint16_t result;
    uint8_t skip_increment = 0;
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
            state->cy = (state->a & 0x01)
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
        case 0x27: break;
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
            result = (uint16_t)state->a += 1;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x07); break;
        case 0x3d:
            result = (uint16_t)state->a -= 1;
            state->a = (uint8_t)(result & 0xff);
            update_flags(state, result, 0x07); break;
        case 0x3e: state->a = op[1]; state->pc += 1; break;
        case 0x3f: state->cy = ~state->cy; break;
        case 0x40: printf("MOV  B B\n"); break;
        case 0x41: printf("MOV  B C\n"); break;
        case 0x42: printf("MOV  B D\n"); break;
        case 0x43: printf("MOV  B E\n"); break;
        case 0x44: printf("MOV  B H\n"); break;
        case 0x45: printf("MOV  B L\n"); break;
        case 0x46: printf("MOV  B M\n"); break;
        case 0x47: printf("MOV  B A\n"); break;
        case 0x48: printf("MOV  C B\n"); break;
        case 0x49: printf("MOV  C C\n"); break;
        case 0x4a: printf("MOV  C D\n"); break;
        case 0x4b: printf("MOV  C E\n"); break;
        case 0x4c: printf("MOV  C H\n"); break;
        case 0x4d: printf("MOV  C L\n"); break;
        case 0x4e: printf("MOV  C M\n"); break;
        case 0x4f: printf("MOV  C A\n"); break;
        case 0x50: printf("MOV  D B\n"); break;
        case 0x51: printf("MOV  D C\n"); break;
        case 0x52: printf("MOV  D D\n"); break;
        case 0x53: printf("MOV  D E\n"); break;
        case 0x54: printf("MOV  D H\n"); break;
        case 0x55: printf("MOV  D L\n"); break;
        case 0x56: printf("MOV  D M\n"); break;
        case 0x57: printf("MOV  D A\n"); break;
        case 0x58: printf("MOV  E B\n"); break;
        case 0x59: printf("MOV  E C\n"); break;
        case 0x5a: printf("MOV  E D\n"); break;
        case 0x5b: printf("MOV  E E\n"); break;
        case 0x5c: printf("MOV  E H\n"); break;
        case 0x5d: printf("MOV  E L\n"); break;
        case 0x5e: printf("MOV  E M\n"); break;
        case 0x5f: printf("MOV  E A\n"); break;
        case 0x60: printf("MOV  H B\n"); break;
        case 0x61: printf("MOV  H C\n"); break;
        case 0x62: printf("MOV  H D\n"); break;
        case 0x63: printf("MOV  H E\n"); break;
        case 0x64: printf("MOV  H H\n"); break;
        case 0x65: printf("MOV  H L\n"); break;
        case 0x66: printf("MOV  H M\n"); break;
        case 0x67: printf("MOV  H A\n"); break;
        case 0x68: printf("MOV  L B\n"); break;
        case 0x69: printf("MOV  L C\n"); break;
        case 0x6a: printf("MOV  L D\n"); break;
        case 0x6b: printf("MOV  L E\n"); break;
        case 0x6c: printf("MOV  L H\n"); break;
        case 0x6d: printf("MOV  L L\n"); break;
        case 0x6e: printf("MOV  L M\n"); break;
        case 0x6f: printf("MOV  L A\n"); break;
        case 0x70: printf("MOV  M B\n"); break;
        case 0x71: printf("MOV  M C\n"); break;
        case 0x72: printf("MOV  M D\n"); break;
        case 0x73: printf("MOV  M E\n"); break;
        case 0x74: printf("MOV  M H\n"); break;
        case 0x75: printf("MOV  M L\n"); break;
        case 0x76: printf("HLT\n"); break;
        case 0x77: printf("MOV  M A\n"); break;
        case 0x78: printf("MOV  A B\n"); break;
        case 0x79: printf("MOV  A C\n"); break;
        case 0x7a: printf("MOV  A D\n"); break;
        case 0x7b: printf("MOV  A E\n"); break;
        case 0x7c: printf("MOV  A H\n"); break;
        case 0x7d: printf("MOV  A L\n"); break;
        case 0x7e: printf("MOV  A M\n"); break;
        case 0x7f: printf("MOV  A A\n"); break;
        case 0x80: printf("ADD  B\n"); break;
        case 0x81: printf("ADD  C\n"); break;
        case 0x82: printf("ADD  D\n"); break;
        case 0x83: printf("ADD  E\n"); break;
        case 0x84: printf("ADD  H\n"); break;
        case 0x85: printf("ADD  L\n"); break;
        case 0x86: printf("ADD  M\n"); break;
        case 0x87: printf("ADD	A\n"); break;
        case 0x88: printf("ADC  B\n"); break;
        case 0x89: printf("ADC  C\n"); break;
        case 0x8a: printf("ADC  D\n"); break;
        case 0x8b: printf("ADC  E\n"); break;
        case 0x8c: printf("ADC  H\n"); break;
        case 0x8d: printf("ADC  L\n"); break;
        case 0x8e: printf("ADC  M\n"); break;
        case 0x8f: printf("ADC  A\n"); break;
        case 0x90: printf("SUB  B\n"); break;
        case 0x91: printf("SUB  C\n"); break;
        case 0x92: printf("SUB  D\n"); break;
        case 0x93: printf("SUB  E\n"); break;
        case 0x94: printf("SUB  H\n"); break;
        case 0x95: printf("SUB  L\n"); break;
        case 0x96: printf("SUB  M\n"); break;
        case 0x97: printf("SUB  A\n"); break;
        case 0x98: printf("SBB  B\n"); break;
        case 0x99: printf("SBB  C\n"); break;
        case 0x9a: printf("SBB  D\n"); break;
        case 0x9b: printf("SBB  E\n"); break;
        case 0x9c: printf("SBB  H\n"); break;
        case 0x9d: printf("SBB  L\n"); break;
        case 0x9e: printf("SBB  M\n"); break;
        case 0x9f: printf("SBB  A\n"); break;
        case 0xa0: printf("ANA  B\n"); break;
        case 0xa1: printf("ANA  C\n"); break;
        case 0xa2: printf("ANA  D\n"); break;
        case 0xa3: printf("ANA  E\n"); break;
        case 0xa4: printf("ANA  H\n"); break;
        case 0xa5: printf("ANA  L\n"); break;
        case 0xa6: printf("ANA  M\n"); break;
        case 0xa7: printf("ANA  A\n"); break;
        case 0xa8: printf("XRA  B\n"); break;
        case 0xa9: printf("XRA  C\n"); break;
        case 0xaa: printf("XRA  D\n"); break;
        case 0xab: printf("XRA  E\n"); break;
        case 0xac: printf("XRA  H\n"); break;
        case 0xad: printf("XRA  L\n"); break;
        case 0xae: printf("XRA  M\n"); break;
        case 0xaf: printf("XRA  A\n"); break;
        case 0xb0: printf("ORA  B\n"); break;
        case 0xb1: printf("ORA  C\n"); break;
        case 0xb2: printf("ORA  D\n"); break;
        case 0xb3: printf("ORA  E\n"); break;
        case 0xb4: printf("ORA  H\n"); break;
        case 0xb5: printf("ORA  L\n"); break;
        case 0xb6: printf("ORA  M\n"); break;
        case 0xb7: printf("ORA  A\n"); break;
        case 0xb8: printf("CMP  B\n"); break;
        case 0xb9: printf("CMP  C\n"); break;
        case 0xba: printf("CMP  D\n"); break;
        case 0xbb: printf("CMP  E\n"); break;
        case 0xbc: printf("CMP  H\n"); break;
        case 0xbd: printf("CMP  L\n"); break;
        case 0xbe: printf("CMP  M\n"); break;
        case 0xbf: printf("CMP  A\n"); break;
        case 0xc0:
        	if(state->z == 0) {
        		state->pc = (state->memory[state->sp + 1] << 8) | (uint8_t)state->memory[state->sp]);
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
                state->pc = (state->memory[state->sp + 1] << 8) | (uint8_t)state->memory[state->sp]);
                state->sp += 2;
            }
            break;
        case 0xc9:
            state->pc = (state->memory[state->sp + 1] << 8) | state->memory[state->sp];
            state->sp += 2;
            skip_increment = 1;
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
        case 0xd0: printf("RNC\n"); break;
        case 0xd1: printf("POP  D\n"); break;
        case 0xd2: printf("JNC  adr\n"); state->pc += 2; break;
        case 0xd3: printf("OUT  0x%02x\n", op[1]); state->pc += 1; break;
        case 0xd4: printf("CNC  adr\n"); state->pc += 2; break;
        case 0xd5: printf("PUSH D\n"); break;
        case 0xd6: printf("SUI  0x%02x\n", op[1]); state->pc += 1; break;
        case 0xd7: printf("RST  2\n"); break;
        case 0xd8: printf("RC\n"); break;
        case 0xd9: break;
        case 0xda: printf("JC   adr\n"); state->pc += 2; break;
        case 0xdb: printf("IN   0x%02x\n", op[1]); state->pc += 1; break;
        case 0xdc: printf("CC   adr\n"); state->pc += 2; break;
        case 0xdd: break;
        case 0xde: printf("SBI  0x%02x\n", op[1]); state->pc += 1; break;
        case 0xdf: printf("RST  3\n"); break;
        case 0xe0: printf("RPO\n"); break;
        case 0xe1: printf("POP  H\n"); break;
        case 0xe2: printf("JPO  adr\n"); state->pc += 2; break;
        case 0xe3: printf("XTHL\n"); break;
        case 0xe4: printf("CPO  adr\n"); state->pc += 2; break;
        case 0xe5: printf("PUSH H\n"); break;
        case 0xe6: printf("ANI  0x%02x\n", op[1]); state->pc += 1; break;
        case 0xe7: printf("RST  4\n"); break;
        case 0xe8: printf("RPE\n"); break;
        case 0xe9: printf("PCHL\n"); break;
        case 0xea: printf("JPE  adr\n"); state->pc += 2; break;
        case 0xeb: printf("XCHX\n"); break;
        case 0xec: printf("CPE  adr\n"); state->pc += 2; break;
        case 0xed: break;
        case 0xee: printf("XRI  0x%02x\n", op[1]); state->pc += 1; break;
        case 0xef: printf("RST  5\n"); break;
        case 0xf0: printf("RP\n"); break;
        case 0xf1: printf("POP  PSW\n"); break;
        case 0xf2: printf("JP   adr\n"); state->pc += 2; break;
        case 0xf3: printf("DI\n"); break;
        case 0xf4: printf("CP   adr\n"); state->pc += 2; break;
        case 0xf5: printf("PUSH PSW\n"); break;
        case 0xf6: printf("ORI  0x%02x\n", op[1]); state->pc += 1; break;
        case 0xf7: printf("RST  6\n"); break;
        case 0xf8: printf("RM\n"); break;
        case 0xf9: printf("SPHL\n"); break;
        case 0xfa: printf("JM   adr\n"); state->pc += 2; break;
        case 0xfb: printf("EI\n"); break;
        case 0xfc: printf("CM   adr\n"); state->pc += 2; break;
        case 0xfd: break;
        case 0xfe: printf("CPI  0x%02x\n", op[1]); state->pc += 1; break;
        case 0xff: printf("RST  7\n"); break;
        default: _unimpl(*op); return 0;
    }
    if(skip_increment == 0)
    	state->pc += 1;
    return 1;
}
