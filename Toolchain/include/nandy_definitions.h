#ifndef NANDY_DEFINITIONS_H
#define NANDY_DEFINITIONS_H

#include <stdint.h>

// Assembly
#define COMMENT_TOK "#"

#define ABSJ_MASK (1<<2)
#define RET_MASK (1<<0)

#define RD_MASK (1<<4)
#define WR_MASK (1<<3)

#define CARRY_SEL_MASK (1<<3)
#define XY_MASK (1<<4)
#define ALU_SEL_MASK (1<<5)

#define COND_MASK (1<<4)

#define MULTIBYTE_MASK (1<<6)


// Instruction bitmasks
// #define IOA_MASK (1<<5)
// #define MEM_WRITE_MASK (1<<5)
// #define PROGFLOW_MASK (1<<4)
// #define MEM_STACK_MASK (1<<4)
// #define COND_MASK (1<<4)
// #define SIG_MASK (1<<3)
// #define ONEOP_MASK (1<<3)
// #define RET_MASK (1<<2)
// #define SCF_MASK (1<<2)
// #define CI_MASK (1<<1)

#define REGID_MASK 0b0111
#define IMM4_MASK 0b1111
#define IMMJ_MASK 0b1110

#define MEM_MASK (1<<7)
#define MMODE_MASK (0b11 << 5)
#define LOAD_MASK (1<< 4)

#define ISR_ADDR 0x7F00

typedef enum alu_mode {
	ALU_B = 0x0,
	ALU_OR = 0x1,
	ALU_AND = 0x2,
	ALU_XOR = 0x3,
	ALU_ADD = 0x4,
	ALU_SUB = 0x5,
	ALU_SL = 0x6,
	ALU_SR = 0x7,
} alu_mode_t;

#define RAM_SIZE (1<<15)
#define ROM_SIZE (1<<15)

// Data sizes
typedef int8_t word_t;
typedef uint16_t addr_t;

typedef enum regid {
	REG_SP,
	REG_IO,
	REG_X,
	REG_Y,
	REG_PL,
	REG_PH,
	REG_QL,
	REG_QH,
	REG_ACC
} regid_t;

typedef enum memmode {
	MM_STACK = (0b00 << 5),
	MM_Q = (0b01 << 5),
	MM_P = (0b10 << 5),
	MM_PPOST = (0b11 << 5)
} memmode_t;

#endif