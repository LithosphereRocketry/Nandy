#include "nandy_instructions.h"
#include "stdio.h"

static const char* asm_nop(const char* text, asm_state_t* state) {
    state->cpu.rom[state->rom_loc] = 0;
    printf("\t\tnop\n");
    state->rom_loc ++;
    return text;
}
static void exe_nop(cpu_state_t* cpu) {
    cpu->pc ++;
}
static void dis_nop(const cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "nop");
}

static void exe_rd(cpu_state_t* cpu) {
    word_t ibyte = peek(cpu, cpu->pc);
    switch(ibyte & 0x11) {
        case 0: cpu->acc = cpu->sp; break;
        case 1: cpu->acc = cpu->ioin; break; // todo: data ping
        case 2: cpu->acc = cpu->dx; break;
        case 3: cpu->acc = cpu->dy; break;
    }
    cpu->pc ++;
}
static void dis_rd(const cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    word_t ibyte = peek(cpu, addr);
    snprintf(buf, len, "rd %s", regnames[ibyte & 0b11]);
}

static void exe__addi(cpu_state_t* cpu) {
    cpu->acc += peek(cpu, cpu->pc + 1);
    cpu->pc += 2;
}

static void dis__addi(const cpu_state_t* cpu, addr_t addr, char* buf, size_t len) {
    snprintf(buf, len, "nop");
}

static const instruction_t default_instrs[] = {
    {
        .mnemonic = "nop",
        .opcode_mask = REGID_MASK,
        .opcode = 0,
        .assemble = asm_nop,
        .disassemble = dis_nop,
        .execute = exe_nop
    }, {
        .mnemonic = "rd",
        .opcode_mask = REGID_MASK,
        .opcode = RD_MASK,
        .disassemble = dis_rd,
        .execute = exe_rd
    }, {
        .mnemonic = "_addi",
        .opcode_mask = 0,
        .opcode = MULTICYCLE_MASK | ALU_SEL_MASK | ALU_ADD,
        .disassemble = dis__addi,
        .execute = exe__addi
    }
};

const ilist_t NANDY_ILIST = {
    .size = sizeof(default_instrs) / sizeof(instruction_t),
    .list = default_instrs
};