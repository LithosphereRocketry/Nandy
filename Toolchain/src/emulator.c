#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

typedef int8_t word_t;
typedef uint16_t addr_t;
typedef uint8_t inst_t;

#define MULTICYCLE_MASK 1<<7
#define ALU_SEL_MASK 1<<6
#define ISP_MASK 1<<5
#define XY_MASK 1<<5
#define MEM_WRITE_MASK 1<<5
#define JUMP_MASK 1<<5
#define CARRY_SEL_MASK 1<<4
#define PROGFLOW_MASK 1<<4
#define MEM_STACK_MASK 1<<4
#define JUMP_COND_MASK 1<<4
#define BRK_MASK 1<<3
#define WR_MASK 1<<3
#define RD_MASK 1<<2
#define COND_INV_MASK 1<<0

#define ALU_INST_MASK 0xF
#define ALU_WRITESBOTH_MASK 0b1100 // any instruction with either of these bits can
								   // write ACC and CARRY simultaneously
#define REGID_MASK 0b0011

#define WRITABLE_REGION (1<<15)


enum ALUMode {
	ALU_B = 0x0,
	ALU_XOR = 0x1,
	ALU_AND = 0x2,
	ALU_OR = 0x3,
	ALU_NE = 0x0,
	ALU_EQ = 0x1,
	ALU_LT = 0x2,
	ALU_GE = 0x3,
	ALU_ADD = 0x4,
	ALU_ADDC = 0x5,
	ALU_SUB = 0x6,
	ALU_SUBC = 0x7,
	ALU_SL = 0x8,
	ALU_SLC = 0x9,
	ALU_SLA = 0xA,
	ALU_SLR = 0xB,
	ALU_SR = 0xC,
	ALU_SRC = 0xD,
	ALU_SRR = 0xE,
	ALU_SRA = 0xF
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
word_t acc, x, y, sp;
bool carry;
addr_t pc = -1;
inst_t mem[65536];
word_t ioin, ioout;

word_t signExt(word_t value, int bits) {
	word_t mask = (-1) << bits;
	if(value & (1 << (bits-1))) {
		return value | mask;
	} else {
		return value & ~mask;
	}
}

bool step(bool interrupt);
bool pause() {
	// PC increments early in this implementation so we subtract 1
	printf("Breakpoint\n");
	while(1) {
		switch(radix) {
			case RADIX_DECIMAL:
				printf("    PC %hhi\t   ACC %hhi\t  IOIN %hhi\tCycles %lu\t Carry %hhi\n  Inst %hhi\t    SP %hhi\t    DX %hhi\t    DY %hhi\tIOOUT %hhi\n> ",
					pc, acc, ioin, cycles, carry, mem[pc], sp, x, y, ioout);
				break;
			case RADIX_UNSIGNED:
				printf("    PC %hhu\t   ACC %hhu\t  IOIN %hhu\tCycles %lu\t Carry %hhi\n  Inst %hhu\t    SP %hhu\t    DX %hhu\t    DY %hhu\tIOOUT %hhu\n> ",
					pc, acc, ioin, cycles, carry, mem[pc], sp, x, y, ioout);
				break;
			case RADIX_HEX:
			default:
				printf("    PC 0x%hhx\t   ACC 0x%hhx\t  IOIN 0x%hhx\tCycles 0x%lx\t Carry %hhi\n  Inst 0x%hhx\t    SP 0x%hhx\t    DX 0x%hhx\t    DY 0x%hhx\tIOOUT 0x%hhx\n> ",
					pc, acc, ioin, cycles, carry, mem[pc], sp, x, y, ioout);
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
		} else {
			printf("Unrecognized command \"%s\"\n", cmd);
		}
	}
}

inst_t fetch() {
	cycles++;
	return mem[++pc];
}

void aluop(enum ALUMode mode, bool isCarry, word_t a, word_t b, word_t* result) {
	bool newcarry = false;
	int newresult = 0;
	switch(mode) {
		case ALU_B:
			newresult = b;
			newcarry = (a != b);
			break;
		case ALU_XOR:
			newresult = a ^ b;
			newcarry = (a == b);
			break;
		case ALU_AND:
			newresult = a & b;
			newcarry = (a < b);
			break;
		case ALU_OR:
			newresult = a | b;
			newcarry = (a >= b);
			break;
		case ALU_ADD:
			newresult = a + b;
			newcarry = (newresult >> 8) & 1;
			break;
		case ALU_ADDC:
			newresult = a + b + (int) carry;
			newcarry = (newresult >> 8) & 1;
			break;
		case ALU_SUB:
			newresult = a - b;
			newcarry = (newresult >> 8) & 1;
			break;
		case ALU_SUBC:
			newresult = a - b + (int) !carry;
			newcarry = (newresult >> 8) & 1;
			break;
		case ALU_SL:
			newresult = a << 1;
			newcarry = (a >> 7) & 1;
			break;
		default:
			printf("ALU operation 0x%x not implemented\n", mode);
	}
	if(isCarry) {
		carry = newcarry;
		if(mode & ALU_WRITESBOTH_MASK) {
			*result = newresult;
		}
	} else {
		*result = newresult;
	}
}

bool step(bool interrupt) {
	inst_t i = fetch();
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
							from = x;
							to = &x;
							break;
						case REGID_DY:
							from = y;
							to = &y;
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
				} else { // prorgam flow
					if(!(i & BRK_MASK)) { // ret/call

					} else { // break
						if(interrupt) return pause();
					}
				}
			} else { // isp instructions
				aluop(ALU_ADD, i & CARRY_SEL_MASK, sp, signExt(i, 4), &sp);
			}
		} else { // normal math
			word_t alub = (i & XY_MASK) ? y : x;
			aluop(i & ALU_INST_MASK, i & CARRY_SEL_MASK, acc, alub, &acc);
		}
	} else { // multicycle operations
		word_t imm = fetch();
		if(!(i & ALU_SEL_MASK)) { // memory 
			addr_t memaddr;
			if(!(i & MEM_STACK_MASK)) {
				memaddr = (((int) y) << 8 | x) + signExt(i, 4);
			} else {
				memaddr = ((unsigned) sp) + signExt(i, 4);
			}
			if(!(i & MEM_WRITE_MASK)) { // reads
				acc = mem[memaddr];
			} else { // writes
				if(memaddr & WRITABLE_REGION) {
					mem[memaddr] = acc;
				}
			}
		} else { // immediates
			if(!(i & JUMP_MASK)) { // immediate ops
				aluop(i & ALU_INST_MASK, i & CARRY_SEL_MASK, acc, imm, &acc);
			} else { // jumps
				if(!(i & JUMP_COND_MASK)) { // unconditional jumps
					int offs = ((((int) signExt(i, 4)) << 8)
							  | (((int) imm) & 0xFF)) - 1;
					// printf("%i %i %i\n", signExt(i, 4), imm, offs);
					pc += offs;
				} else {
					if((carry && !(i & COND_INV_MASK)) || (!carry && (i & COND_INV_MASK))) {
						pc += imm;
					}
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
	size_t loaded = fread(mem, sizeof(inst_t), 32768, binfile);
	printf("%lu bytes loaded\n", loaded);
	// A bit of a hack to start in the debug menu but then continue running with
	// no debug afterward
	if(pause()) {
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