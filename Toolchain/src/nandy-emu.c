#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <time.h>

#include "nandy_tools.h"

#ifdef __unix__
#include <unistd.h>
#endif

// TODO: finish refactoring
extern bool parity(word_t w);

#define ALU_INST_MASK 0xF
#define ALU_WRITESBOTH_MASK 0b1000 // any instruction with either of these bits can
								   // write ACC and CARRY simultaneously
#define WRITABLE_REGION (1<<15)

unsigned int cpufreq = 1000000;
unsigned int baudrate = 1200;

#define SIGNAL_MASK 0x7
enum Signal {
	SIGNAL_BRK = 00,
	SIGNAL_BEL = 01,
	SIGNAL_DINT = 04,
	SIGNAL_EINT = 05,
	SIGNAL_ICLR = 06,
	SIGNAL_ISET = 07
};

enum {
	RADIX_DECIMAL,
	RADIX_UNSIGNED,
	RADIX_HEX
} radix = RADIX_HEX;

enum RegID {
	REGID_SP = 0x0,
	REGID_IO = 0x1,
	REGID_DX = 0x2,
	REGID_DY = 0x3
};

unsigned long cycles = -1;
word_t acc, x, y, sp, ix, iy;
bool carry;
addr_t pc = -1;
word_t mem[65536];
word_t ioin, ioout;
bool intEnabled;
bool isInterrupt;

unsigned long ioCooldown;

word_t signExt(word_t value, int bits) {
	word_t mask = (-1) << bits;
	if(value & (1 << (bits-1))) {
		return value | mask;
	} else {
		return value & ~mask;
	}
}

bool wantsInterrupt() {
	if(ioCooldown == 0) {
		int nextChar = getc(stdin);
		if(nextChar == EOF) {
			return false;
		} else {
			ioin = nextChar;
			ioCooldown = (cpufreq / baudrate);
			return true;
		}
	} else {
		return false;
	}
}

bool step(bool debugint);
bool pauseToDbg() {
	printf("\nBreakpoint\n");
	while(1) {
		switch(radix) {
			case RADIX_DECIMAL:
				printf("    PC %hi\t   ACC %hhi\t  IOIN %hhi\tCycles %lu\t Carry %hhi\n"\
					   "Inst %hhi\t    SP %hhi\t    DX %hhi\t    DY %hhi\tIOOUT %hhi\n"\
					   "DEBUG> ",
					pc, acc, ioin, cycles, carry, mem[pc], sp,
					isInterrupt ? ix : x, isInterrupt ? iy : y, ioout);
				break;
			case RADIX_UNSIGNED:
				printf("    PC %hu\t   ACC %hhu\t  IOIN %hhu\tCycles %lu\t Carry %hhi\n"\
					   "Inst %hhu\t    SP %hhu\t    DX %hhu\t    DY %hhu\tIOOUT %hhu\n"\
					   "DEBUG> ",
					pc, acc, ioin, cycles, carry, mem[pc], sp,
					isInterrupt ? ix : x, isInterrupt ? iy : y, ioout);
				break;
			case RADIX_HEX:
			default:
				printf("    PC 0x%hx\t   ACC 0x%hhx\t  IOIN 0x%hhx\tCycles 0x%lx\t Carry %hhi\n"\
					   "Inst 0x%hhx\t    SP 0x%hhx\t    DX 0x%hhx\t    DY 0x%hhx\tIOOUT 0x%hhx\n"\
					   "DEBUG> ",
					pc, acc, ioin, cycles, carry, mem[pc], sp,
					isInterrupt ? ix : x, isInterrupt ? iy : y, ioout);
		}
		char input[256];
		fgets(input, 255, stdin);
		char cmd[64];
		sscanf(input, "%63s", cmd);
		if(!strcmp(cmd, "quit") || !strcmp(cmd, "q")) {
			return false;
		} else if(!strcmp(cmd, "step") || !strcmp(cmd, "s")) {
			step(false);
		} else if(!strcmp(cmd, "continue") || !strcmp(cmd, "c")) {
			return true;
		} else if(!strcmp(cmd, "radix") || !strcmp(cmd, "r")) {
			char radtxt[64];
			if(sscanf(input, "%*s %63s", radtxt) < 1) {
				printf("No radix mode given\n");
			} else if(!strcmp(radtxt, "decimal") || !strcmp(radtxt, "signed") || !strcmp(radtxt, "d") || !strcmp(radtxt, "s")) {
				radix = RADIX_DECIMAL;
			} else if(!strcmp(radtxt, "unsigned") || !strcmp(radtxt, "u")) {
				radix = RADIX_UNSIGNED;
			} else if(!strcmp(radtxt, "hexadecimal") || !strcmp(radtxt, "hex") || !strcmp(radtxt, "h") || !strcmp(radtxt, "x")) {
				radix = RADIX_HEX;
			} else {
				printf("Unrecognized radix mode \"%s\"\n", radtxt);
			}
		} else if(!strcmp(cmd, "io") || !strcmp(cmd, "i")) {
			long newio = 0;
			if(sscanf(input, "%*s %li", &newio) < 1) {
				printf("No I/O value given\n");
			} else if((radix == RADIX_DECIMAL || radix == RADIX_HEX) && newio >= -128 && newio <= 127) {
				ioin = newio;
			} else if((radix == RADIX_UNSIGNED || radix == RADIX_HEX) && newio >= 0 && newio <= 255) {
				ioin = newio;
			} else {
				printf("Warning: IO value %li will be truncated\n", newio);
				ioin = newio;
			}
		} else if(!strcmp(cmd, "peek") || !strcmp(cmd, "p")) {
			long peekaddr = 0;
			if(sscanf(input, "%*s %lx", &peekaddr) < 1) {
				printf("No I/O value given\n");
			} else {
				printf("mem[%hx] = 0x%hhx\n", (short) peekaddr, mem[peekaddr & 0xFFFF]);
			}
		} else {
			printf("Unrecognized command \"%s\"\n", cmd);
		}
	}
}

word_t fetch() {
#ifdef __unix__
	// We assume the actual emulator takes zero time at all
	// we can probably get better timing precision by using a different timing
	// method but this is pretty OK
	// update: this sucks
	// static const struct timespec us = {0, 1000};
	// nanosleep(&us, NULL);
#endif
	if(ioCooldown != 0) { ioCooldown--; }
	cycles++;
	return mem[++pc];
}

void aluop(enum ALUMode mode, bool isCarry, bool isXY, word_t a, word_t b, word_t* result) {
	bool newcarry = false;
	int newresult = 0;
	switch(mode) {
		case ALU_B:
			newresult = b;
			newcarry = a >> 7;
			break;
		case ALU_OR:
			newresult = a | b;
			newcarry = carry;
			break;
		case ALU_AND:
			newresult = a & b;
			newcarry = (a != 0);
			break;
		case ALU_XOR:
			newresult = a ^ b;
			newcarry = parity(a);
			break;
		case ALU_NB:
			newresult = ~b;
			newcarry = !(a >> 7);
			break;
		case ALU_NOR:
			newresult = ~(a | b);
			newcarry = !carry;
			break;
		case ALU_NAND:
			newresult = ~(a & b);
			newcarry = (a == 0);
			break;
		case ALU_XNOR:
			newresult = ~(a ^ b);
			newcarry = !parity(a);
			break;
		case ALU_ADD:
			newresult = (((int) a) & 0xFF) + (((int) b) & 0xFF);
			newcarry = (newresult >> 8) & 1;
			break;
		case ALU_ADDC:
			newresult = (((int) a) & 0xFF) + (((int) b) & 0xFF) + ((int) carry);
			newcarry = (newresult >> 8) & 1;
			break;
		case ALU_SUB:
			newresult = (((int) a) & 0xFF) + 256 - (((int) b) & 0xFF);
			newcarry = (newresult >> 8) & 1;
			break;
		case ALU_SUBC:
			newresult = (((int) a) & 0xFF) + 255 + ((int) carry)
					- (((int) b) & 0xFF);
			newcarry = (newresult >> 8) & 1;
			break;
		case ALU_SL:
			if(!isXY) {
				newresult = a << 1;
				newcarry = (a >> 7) & 1;
			} else {
				newresult = (a >> 1) & 0x7F;
				newcarry = a & 1;
			}
			break;
		case ALU_SLC:
			if(!isXY) {
				newresult = a << 1 | (carry ? 1 : 0);
				newcarry = (a >> 7) & 1;
			} else {
				newresult = ((a >> 1) & 0x7F) | (carry ? 0x80 : 0);
				newcarry = a & 1;
			}
			break;
		default:
			printf("ALU operation 0x%x not implemented\n", mode);
	}
	if(isCarry) {
		carry = newcarry;
		if(mode & ALU_WRITESBOTH_MASK) {
			*result = newresult & 0xFF;
		}
	} else {
		*result = newresult & 0xFF;
	}
}

bool step(bool debugint) {
	if(intEnabled && !isInterrupt && wantsInterrupt()) {
		isInterrupt = true;
		ix = (pc+1) & 0xFF;
		iy = (pc+1) >> 8;
		pc = 0x7EFF;
	}

	word_t i = fetch();
	if(!(i & MULTICYCLE_MASK)) { // single cycle operations
		if(!(i & ALU_SEL_MASK)) { // basic instructions
			if(!(i & ISP_MASK)) { // housekeeping/program logic
				if(!(i & PROGFLOW_MASK)) { // Register swaps
					word_t from;
					word_t* to;
					switch(i & REGID_MASK) {
						case REGID_SP:
							from = sp;
							to = &sp;
							break;
						case REGID_IO:
							from = ioin;
							to = &ioout;
							break;
						case REGID_DX:
							from = isInterrupt ? ix : x;
							to = isInterrupt ? &ix : &x;
							break;
						case REGID_DY:
							from = isInterrupt ? iy : y;
							to = isInterrupt ? &iy : &y;
							break;
					}
					if(i & WR_MASK) {
						*to = acc;
						if(to == &ioout) {
							putc(acc, stdout);
						}
					}
					if(i & RD_MASK) {
						acc = from;
					}
				} else { // program flow
					if(!(i & SIG_MASK)) { // ret/call
						addr_t oldpc = pc + 1;
						pc = (((addr_t) (isInterrupt ? iy : y)) << 8)
						   + (isInterrupt ? ix : x) - 1;
						if(i & RET_MASK) { // call only
							*(isInterrupt ? &ix : &x) = oldpc & 0xFF;
							*(isInterrupt ? &iy : &y) = oldpc >> 8;
						}
						if(i & CI_MASK) { // jri only
							isInterrupt = false;
						}
					} else { // signal
						switch(i & SIGNAL_MASK) {
							case SIGNAL_BRK: if(debugint) return pauseToDbg(); break;
							case SIGNAL_BEL: putchar('\a'); break;
							case SIGNAL_DINT: intEnabled = false; break;
							case SIGNAL_EINT: intEnabled = true; break;
							case SIGNAL_ICLR: isInterrupt = false; break;
							case SIGNAL_ISET: isInterrupt = true; break;
						}
					}
				}
			} else { // isp instructions
				aluop(ALU_ADD, i & CARRY_SEL_MASK, i & XY_MASK, sp, signExt(i, 4), &sp);
			}
		} else { // normal math
			word_t alub = (i & XY_MASK) ? (isInterrupt ? iy : y)
										: (isInterrupt ? ix : x);
			aluop(i & ALU_INST_MASK, i & CARRY_SEL_MASK, i & XY_MASK, acc, alub, &acc);
		}
	} else { // multicycle operations
		if(!(i & ALU_SEL_MASK)) { // memory 
			cycles++; // These operations don't move PC but they do consume a cycle
			addr_t memaddr;
			if(!(i & MEM_STACK_MASK)) {
				memaddr = (((int) (isInterrupt ? iy : y)) << 8 
								| (isInterrupt ? ix : x)) + (i & 0xF);
			} else {
				memaddr = 0xFF00 + (((int) sp) & 0xFF)  + (i & 0xF);
			}
			if(!(i & MEM_WRITE_MASK)) { // reads
				acc = mem[memaddr];
			} else { // writes
				if(memaddr & WRITABLE_REGION) {
					mem[memaddr] = acc;
				}
			}
		} else { // immediates
			word_t imm = fetch();
			if(!(i & JUMP_MASK)) { // immediate ops
				aluop(i & ALU_INST_MASK, i & CARRY_SEL_MASK, i & XY_MASK, acc, imm, &acc);
			} else { // jumps
				int offs = ((((int) signExt(i, 4)) << 8)
							| (((int) imm) & 0xFF)) - 1;
				if(!((i & COND_MASK) && carry)) {
					pc += offs;
				}
			}
		}
	}
	return true;
}

int main(int argc, char** argv) {
	if(argc != 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		return 1;
	}
    char* path = argv[1];
	printf("Loading file %s...\n", path);
	FILE* binfile = fopen(path, "rb");
	size_t loaded = fread(mem, sizeof(word_t), 32768, binfile);
	printf("%lu bytes loaded\n", loaded);
	// A bit of a hack to start in the debug menu but then continue running with
	// no debug afterward
	if(pauseToDbg()) {
		while(step(true));
	}
	// char* path = NULL;
    // bool debug = false;
    // for(int i = 1; i < argc; i++) {
	// 	if(argv[i][0] == '-') {
	// 		for(int j = 1; argv[i][j] != '\0'; j++) {
	// 			switch(argv[i][j]) {
	// 				case 'd':
	// 					debug = true;
	// 					break;
	// 				default:
	// 					printf("Unexpected flag %c\n", argv[i][j]);
	// 					return 1;
	// 			}
	// 		}
	// 	} else {
	// 		if(!path) {
	// 			path = argv[i];
	// 		} else {
	// 			printf("Unexpected argument %s\n", argv[i]);
	// 			return 1;
	// 		}
	// 	}
	// }
}