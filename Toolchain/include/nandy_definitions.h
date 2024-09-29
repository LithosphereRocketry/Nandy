#ifndef NANDY_DEFINITIONS_H
#define NANDY_DEFINITIONS_H

#include <stdint.h>

// Assembly
#define COMMENT_TOK "#"

// Instruction bitmasks
#define ADDR_RAM_MASK (1<<15)
#define MULTICYCLE_MASK (1<<7)
#define ALU_SEL_MASK (1<<6)
#define IOA_MASK (1<<5)
#define XY_MASK (1<<5)
#define MEM_WRITE_MASK (1<<5)
#define JUMP_MASK (1<<5)
#define CARRY_SEL_MASK (1<<4)
#define PROGFLOW_MASK (1<<4)
#define MEM_STACK_MASK (1<<4)
#define COND_MASK (1<<4)
#define SIG_MASK (1<<3)
#define WR_MASK (1<<3)
#define ONEOP_MASK (1<<3)
#define RD_MASK (1<<2)
#define RET_MASK (1<<2)
#define SCF_MASK (1<<2)
#define CI_MASK (1<<1)

#define REGID_MASK 0b0011
#define IMM4_MASK 0b1111
#define IMM5_MASK 0b11111
#define INC_MASK 0b111

#define ISR_ADDR 0x7F00

typedef enum alu_mode {
	ALU_B = 0x0,
	ALU_OR = 0x1,
	ALU_AND = 0x2,
	ALU_XOR = 0x3,
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
} alu_mode_t;

#define RAM_SIZE (1<<15)
#define ROM_SIZE (1<<15)

// Data sizes
typedef int8_t word_t;
typedef uint16_t addr_t;

typedef enum regid {
	REG_SP,
	REG_IO,
	REG_DX,
	REG_DY,
	REG_ACC
} regid_t;

typedef enum sigid {
	SIG_BRK = 0,
	SIG_BELL = 1,
	SIG_CSCLR = 2,
	SIG_CSSET = 3,
	SIG_DINT = 4,
	SIG_EINT = 5,
	SIG_ICLR = 6,
	SIG_ISET = 7
} sigid_t;

#endif