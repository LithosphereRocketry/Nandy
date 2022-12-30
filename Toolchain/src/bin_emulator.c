#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

typedef int8_t word_t;
typedef uint8_t inst_t;

enum {
	RADIX_DECIMAL,
	RADIX_UNSIGNED,
	RADIX_HEX
} radix = RADIX_HEX;

unsigned long cycles = -1;
word_t acc, data, sp;
bool carry;
word_t pc = -1;
word_t ram[256];
const inst_t rom[256] = {
	0b11001110, // 	brk
	0b01010100, // 	lui 10
	0b00000100, // 	lui 0
	0b11000001, // 	swd
	0b00000110, // 	andi 0
	// loop:
	0b11110000, // 	add
	0b11000001, // 	swd
	0b00001011, // 	subi 1
	0b11000001, // 	swd
	0b11001011, // 	jnif ...
	5, // loop
	0b11000100, // 	ioa
	0b11001110  // 	brk
};
word_t ioain, iobin;
word_t ioaout, iobout;

word_t signExt(word_t value, int bits) {
	word_t mask = (-1) << bits;
	if(value & (1 << (bits-1))) {
		return value | mask;
	} else {
		return value;
	}
}

bool step(bool interrupt);
bool pause() {
	// PC increments early in this implementation so we subtract 1
	printf("Breakpoint\n");
	while(1) {
		switch(radix) {
			case RADIX_DECIMAL:
				printf("    PC %hhi\tCycles %hhi\t   ACC %hhi\t IOAIN %hhi\t IOBIN %hhi\n  Inst %hhi\t    SP %hhi\t  DATA %hhi\tIOAOUT %hhi\tIOBOUT %hhi\n> ",
					pc, cycles, acc, ioain, iobin, rom[pc], sp, data, ioaout, iobout);
				break;
			case RADIX_UNSIGNED:
				printf("    PC %hhu\tCycles %hhu\t   ACC %hhu\t IOAIN %hhu\t IOBIN %hhu\n  Inst %hhu\t    SP %hhu\t  DATA %hhu\tIOAOUT %hhu\tIOBOUT %hhu\n> ",
					pc, cycles, acc, ioain, iobin, rom[pc], sp, data, ioaout, iobout);
				break;
			case RADIX_HEX:
			default:
				printf("    PC 0x%hhx\tCycles 0x%hhx\t   ACC 0x%hhx\t IOAIN 0x%hhx\t IOBIN 0x%hhx\n  Inst 0x%hhx\t    SP 0x%hhx\t  DATA 0x%hhx\tIOAOUT 0x%hhx\tIOBOUT 0x%hhx\n> ",
					pc, cycles, acc, ioain, iobin, rom[pc], sp, data, ioaout, iobout);
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
		} else {
			printf("Unrecognized command \"%s\"\n", cmd);
		}
	}
}

inst_t fetch() {
	cycles++;
	return rom[++pc];
}

bool step(bool interrupt) {
	inst_t op = fetch();
	if((op & 0b10000000) == 0) { // no first bit = 4 bit imm
		word_t imm = (op & 0b01111000) >> 3;
		word_t result;
		switch(op & 0b111) {
			case 0: // isp
				result = ((int) sp) + imm;
				sp = result;
				carry = (result >> 8) & 1;
				break;
			case 1: // addi
				result = ((int) acc) - imm;
				acc = result;
				carry = (result >> 8) & 1;
				break;
			case 2: // dsp
				result = ((int) sp) - imm;
				sp = result;
				carry = (result >> 8) & 1;
				break;
			case 3: // subi
				result = ((int) acc) - imm;
				acc = result;
				carry = (result >> 8) & 1;
				break;
			case 4: // lui
				carry = acc != signExt(imm, 4);
				acc = ((acc >> 4) & 0xF) | imm << 4;
				break;
			case 5: // xori
				carry = acc == signExt(imm, 4);
				acc ^= signExt(imm, 4);
				break;
			case 6: // andi
				carry = acc < signExt(imm, 4);
				acc &= signExt(imm, 4);
				break;
			case 7: // ori
				carry = acc >= signExt(imm, 4);
				acc |= signExt(imm, 4);
				break;
		}
	} else if((op & 0b01000000) == 0) { // no second bit = 3 bit imm
		int8_t imm = (op & 0b00111000) >> 3;
		switch(op & 0b111) {
			case 0: // stra
				ram[data + imm] = acc;
				break;
			case 1: // strs
				ram[sp + imm] = acc;
				break;
			case 2: // stran
				ram[data - imm] = acc;
				break;
			case 3: // strsn
				ram[sp - imm] = acc;
				break;
			case 4: // lda
				acc = ram[data + imm];
				break;
			case 5: // lds
				acc = ram[sp + imm];
				break;
			case 6: // ldan
				acc = ram[data - imm];
				break;
			case 7: // ldsn
				acc = ram[sp - imm];
				break;
		}
	} else if((op & 0b00110000) == 0) { // no third and fourth bits = special instruction
		if((op & 0b00001000) == 0) { // no fifth bit = register transfer
			word_t temp;
			switch(op & 0b111) {
				case 0: // wrd
					data = acc;
					break;
				case 1: // swd
					temp = acc;
					acc = data;
					data = temp;
					break;
				case 2: // wrs
					sp = acc;
					break;
				case 3: // sws
					temp = acc;
					acc = sp;
					sp = temp;
					break;
				case 4: // ioa
					ioaout = acc;
					acc = ioain;
					break;
				case 5: // rdd
					acc = data;
					break;
				case 6: // iob
					iobout = acc;
					acc = iobin;
					break;
				case 7: // rds
					acc = sp;
					break;
			}
		} else { // has fifth bit = jumps and weirdness
			if((op & 0b00000100) == 0) { // no sixth bit = multicycle
				inst_t datafield = fetch();
				switch(op & 0b11) {
					case 0: // j
						pc = datafield-1;
						break;
					case 1: // jsr
						data = pc+1;
						pc = datafield-1;
						break;
					case 2: // jif
						if(carry) {
							pc = datafield-1;
						}
						break;
					case 3: // jnif
						if(!carry) {
							pc = datafield-1;
						}
						break;
				}
			} else { // single cycle program flow
				switch(op & 0b11) {
					case 0: // ret
						pc = data;
						break;
					case 1: // retsr
						word_t temp = pc+1;
						pc = data-1;
						data = temp;
						break;
					case 2: // brk
						if(interrupt) {
							return pause();
						}
						break;
					case 3: // nop
						break;
				}
			}
		}
	} else { // ALU operations
		bool carryRes;
		word_t result;
		int longres;
		switch(op & 0b00001111) {
			case 0: // add
				longres = acc + data;
				result = longres & 0xFF;
				carryRes = (longres >> 8) & 1;
				break;
			case 1: // addc
				longres = acc + data + carry;
				result = longres & 0xFF;
				carryRes = (longres >> 8) & 1;
				break;
			case 2: // sub
				longres = acc + ~data + 1; // this is the same as acc - data
				result = longres & 0xFF;
				carryRes = (longres >> 8) & 1;
				break;
			case 3: // subc
				longres = acc + ~data + carry;
				result = longres & 0xFF;
				carryRes = (longres >> 8) & 1;
				break;
			case 4: // lu
				result = ((acc >> 4) & 0xF) | data << 4;
				carryRes = acc != data;
				break;
			case 5: // xor
				result = acc ^ data;
				carryRes = acc == data;
				break;
			case 6: // and
				result = acc & data;
				carryRes = acc < data;
				break;
			case 7: // or
				result = acc | data;
				carryRes = acc >= data;
				break;
			case 8: // sl
				carryRes = (acc >> 7) & 1; 
				result = acc << 1;
				break;
			case 9: // slc
				carryRes = (acc >> 7) & 1;
				result = (acc << 1) | carry;
				break;
			case 0xA: // slr
				carryRes = (acc >> 7) & 1;
				result = (acc << 1) | carryRes;
				break;
			case 0xB: // sla
				carryRes = (acc >> 7) & 1;
				result = (acc << 1) | (acc & 1);
				break;
			case 0xC: // sr
				carryRes = acc & 1;
				result = (acc >> 1) & 0b01111111;
				break;
			case 0xD: // src
				carryRes = acc & 1;
				result = ((acc >> 1) & 0b01111111) | (((int)carry) << 7);
				break;
			case 0xE: // srr
				carryRes = acc & 1;
				result = ((acc >> 1) & 0b01111111) | (((int)carryRes) << 7);
				break;
			case 0xF: // sra
				carryRes = acc & 1;
				result = acc >> 1;
				break;
		}
		if(op & 0b00100000) {
			carry = carryRes;
		}
		if(op & 0b00010000) {
			acc = result;
		}
	}
	return true;
}

int main(int argc, char** argv) {
	while(step(true));
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