#include <string.h>
#include <stddef.h>

#include "nandy_parse_tools.h"
#include "nandy_check_tools.h"
#include "nandy_instr_defs.h"
#include "nandy_tools.h"

#if DEBUG_PRINT_CTRL_GRAPH
    #include "nandy_emu_tools.h"
    #include <stdio.h>
#endif

static void maybeInitCtrlGraph(ctrl_graph_t* graph) {
    if(!graph->block_sz) {
        memset(graph, 0, sizeof(ctrl_graph_t));
        
        graph->block_cap = 4;
        graph->blocks = malloc(graph->block_cap * sizeof(ctrl_block_t));
        
        // Index 0 is the null index
        memset(graph->blocks, 0, sizeof(ctrl_block_t));
        graph->block_sz = 1;
    }
}

static size_t findCtrlBlockSlot(ctrl_graph_t* graph, addr_t pc) {
    size_t l = 1, r = graph->block_sz, i = 1;
    while(l < r) {
        i = l + (r - l) / 2;
        addr_t block_pc = graph->blocks[i].block_pc;
        if(pc > block_pc)
            l = i + 1;
        else if(pc < block_pc)
            r = i;
        else return i;
    }
    return l;
}

static size_t findOrAddCtrlBlock(ctrl_graph_t* graph, addr_t pc, ctrl_block_op_t *op) {
    if(graph->block_sz >= graph->block_cap) {
        graph->block_cap = graph->block_sz * 2;
        graph->blocks = realloc(graph->blocks, graph->block_cap * sizeof(ctrl_graph_t));
    }
    
    size_t block_idx;
    ctrl_block_t *block, *prev;
    // If pc is greater, we'll insert on the right, so no need to binary search.
    // This is by far the most common case, so it's an important optimization.
    if(pc > graph->blocks[graph->block_sz-1].block_pc) {
        block_idx = graph->block_sz;
        block = &graph->blocks[block_idx];
        
        if(op) *op = CTRL_BLOCK_APPENDED;
    } else {
        block_idx = findCtrlBlockSlot(graph, pc);
        block = &graph->blocks[block_idx];
        
        // The block already exists
        if(block->block_pc == pc) {
            if(op) *op = CTRL_BLOCK_FOUND;
            return block_idx;
        }
        
        // Shift over the blocks and update references
        if(graph->current_idx >= block_idx)
            graph->current_idx++;
        for(size_t i = 0; i < graph->block_sz; i++) {
            if(graph->blocks[i].next_idx >= block_idx)
                graph->blocks[i].next_idx++;
            if(graph->blocks[i].branch_idx >= block_idx)
                graph->blocks[i].branch_idx++;
        }
        size_t move_size = (graph->block_sz - block_idx) * sizeof(ctrl_block_t);
        memmove(block+1, block, move_size);
        
        if(op) *op = CTRL_BLOCK_INSERTED;
    }
    
    memset(block, 0, sizeof(ctrl_block_t));
    block->block_pc = pc;
    graph->block_sz++;
    
    // If we're within the previous block, split it
    prev = block - 1;
    int delta_pc = prev->block_pc + prev->block_loc - pc;
    if(delta_pc > 0) {
        block->block_loc = delta_pc;
        prev->block_loc -= delta_pc;
        block->next_idx = prev->next_idx;
        block->branch_idx = prev->branch_idx;
        prev->next_idx = block_idx;
        prev->branch_idx = 0;
        block->refcount++;
    }
    
    return block_idx;
}

void updateNextCtrlLink(ctrl_graph_t* graph, addr_t end_pc, size_t next_idx) {
    maybeInitCtrlGraph(graph);
    if(graph->current_idx) {
        ctrl_block_t* current = &graph->blocks[graph->current_idx];
        current->block_loc = end_pc - current->block_pc;
        if(!current->next_idx && next_idx) {
            graph->blocks[next_idx].refcount++;
        }
        current->next_idx = next_idx;
    }
    graph->current_idx = next_idx;
}

void addFloatingCtrlBlock(ctrl_graph_t* graph, addr_t pc) {
    maybeInitCtrlGraph(graph);
    findOrAddCtrlBlock(graph, pc, NULL);
}

void addNextCtrlBlock(ctrl_graph_t* graph, addr_t pc, bool is_linked) {
    maybeInitCtrlGraph(graph);
    size_t block_idx = findOrAddCtrlBlock(graph, pc, NULL);
    updateNextCtrlLink(graph, pc, is_linked ? block_idx : 0);
    graph->current_idx = block_idx;
}

void addBranchCtrlBlock(ctrl_graph_t* graph, addr_t origin_pc, addr_t target_pc) {
    // NOTE: This must be called only after the origin block has been added
    if(origin_pc < graph->blocks[1].block_pc) return;
    maybeInitCtrlGraph(graph);
    
    size_t target_idx = findOrAddCtrlBlock(graph, target_pc, NULL);
    size_t origin_idx = findCtrlBlockSlot(graph, origin_pc);
    // We want the block at or before origin_pc
    if(origin_pc < graph->blocks[origin_idx].block_pc) {
        origin_idx--;
    }
    
    graph->blocks[origin_idx].branch_idx = target_idx;
    graph->blocks[target_idx].refcount++;
}

#if DEBUG_PRINT_CTRL_GRAPH
    void debugPrintCtrlGraph(asm_state_t* state) {
        char buf[32];
        cpu_state_t cpu = {0};
        memcpy(cpu.rom, state->rom, sizeof(state->rom));
        
        ctrl_graph_t* graph = &state->ctrl_graph;
        printf("\n[Control Graph -- %ld of %ld blocks", graph->block_sz, graph->block_cap);
        if(graph->current_idx)
            printf(" :: Current -> %ld", graph->current_idx);
        printf("]\n");
        
        for(size_t i = 0; i < graph->block_sz; i++) {
            ctrl_block_t* block = &graph->blocks[i];
            printf("[Block %ld -- %04X (%d bytes) (refcount %d)", i, block->block_pc, block->block_loc, block->refcount);
            if(block->next_idx)
                printf(" :: Next -> %ld", block->next_idx);
            if(block->branch_idx)
                printf(" :: Branch -> %ld", block->branch_idx);
            printf("]\n");
            
            int nops = 0;
            addr_t pc = block->block_pc;
            while(pc < block->block_pc + block->block_loc) {
                if(!state->rom[pc] && (nops || (pc < state->rom_loc-1 && !state->rom[pc+1]))) {
                    nops++;
                } else {
                    if(nops) {
                        printf("\t%04X - %04x: nop (x%d)\n", pc - nops, pc - 1, nops);
                        nops = 0;
                    }
                    
                    const instruction_t* instr = ilookup(state->rom[pc]);
                    cpu.pc = pc;
                    instr->disassemble(instr, &cpu, pc, buf, sizeof(buf));
                    
                    printf("\t%04X:\t%s\n", pc, buf);
                }
                
                pc += nbytes(state->rom[pc]);
            }
        }
        
        printf("[Control Graph End]\n\n");
    }
#endif

// static bool isRegWr(word_t opcode, regid_t regid) {
//     const instruction_t* instr = ilookup(opcode);
//     return (instr->opcode == i_wr.opcode || instr->opcode == i_sw.opcode)
//         && (opcode & instr->opcode_mask) == (int) regid;
// }

static bool regIsKnown(const static_state_t* state, regid_t reg) {
    switch(reg) {
        case REG_ACC: return state->flags_values & STATIC_ACC;
        case REG_SP: return state->flags_values & STATIC_SP;
        case REG_DX: return state->flags_values & STATIC_DX;
        case REG_DY: return state->flags_values & STATIC_DY;
        case REG_IO: return false;
    }
    return false; // unreachable unless you feed it garbage
}

static void regSetKnown(static_state_t* state, regid_t reg) {
    switch(reg) {
        case REG_ACC: state->flags_values |= STATIC_ACC; break;
        case REG_SP: state->flags_values |= STATIC_SP; break;
        case REG_DX: state->flags_values |= STATIC_DX; break;
        case REG_DY: state->flags_values |= STATIC_DY; break;
        case REG_IO: break;
    }
}
static void regSetUnknown(static_state_t* state, regid_t reg) {
    switch(reg) {
        case REG_ACC: state->flags_values &= ~STATIC_ACC; break;
        case REG_SP: state->flags_values &= ~STATIC_SP; break;
        case REG_DX: state->flags_values &= ~STATIC_DX; break;
        case REG_DY: state->flags_values &= ~STATIC_DY; break;
        case REG_IO: break;
    }
}

static void moveReg(static_state_t* state, regid_t src, regid_t dst) {
    printf("%s -> %s\n", regnames[src][0], regnames[dst][0]);
    if(regIsKnown(state, src)) {
        regSetKnown(state, dst);
        word_t value;
        switch(src) {
            case REG_ACC: value = state->cpu.acc; break;
            case REG_SP: value = state->cpu.sp; break;
            case REG_DX: value = state->cpu.dx; break;
            case REG_DY: value = state->cpu.dy; break;
            case REG_IO: default: value = 0; break;
        }
        switch(dst) {
            case REG_ACC: state->cpu.acc = value; break;
            case REG_SP: state->cpu.sp = value; break;
            case REG_DX: state->cpu.dx = value; break;
            case REG_DY: state->cpu.dy = value; break;
            case REG_IO: break;
        }
    } else regSetUnknown(state, dst);
}

static void swapReg(static_state_t* state, regid_t src, regid_t dst) {
    bool srcKnown = regIsKnown(state, src);
    word_t srcValue;
    switch(src) {
        case REG_ACC: srcValue = state->cpu.acc; break;
        case REG_SP: srcValue = state->cpu.sp; break;
        case REG_DX: srcValue = state->cpu.dx; break;
        case REG_DY: srcValue = state->cpu.dy; break;
        case REG_IO: default: srcValue = 0; break;
    }

    bool dstKnown = regIsKnown(state, dst);
    word_t dstValue;
    switch(dst) {
        case REG_ACC: dstValue = state->cpu.acc; break;
        case REG_SP: dstValue = state->cpu.sp; break;
        case REG_DX: dstValue = state->cpu.dx; break;
        case REG_DY: dstValue = state->cpu.dy; break;
        case REG_IO: default: dstValue = 0; break;
    }

    if(srcKnown) {
        regSetKnown(state, dst);
        switch(dst) {
            case REG_ACC: state->cpu.acc = srcValue; break;
            case REG_SP: state->cpu.sp = srcValue; break;
            case REG_DX: state->cpu.dx = srcValue; break;
            case REG_DY: state->cpu.dy = srcValue; break;
            case REG_IO: break;
        }
    } else regSetUnknown(state, dst);

    if(dstKnown) {
        regSetKnown(state, src);
        switch(src) {
            case REG_ACC: state->cpu.acc = dstValue; break;
            case REG_SP: state->cpu.sp = dstValue; break;
            case REG_DX: state->cpu.dx = dstValue; break;
            case REG_DY: state->cpu.dy = dstValue; break;
            case REG_IO: break;
        }
    } else regSetUnknown(state, src);
}

static void staticCheckBlock(asm_state_t* code, static_state_t* state) {
    ctrl_block_t* block = state->block;
    
    for(int i = 0; i < block->block_loc;) {
        addr_t pc = block->block_pc + i;
        word_t opcode = code->rom[pc];
        const instruction_t* instr = ilookup(opcode);
        
        if(instr == &i_eint) {
            state->flags_values |= STATIC_INT_EN;
            state->cpu.int_en = true;
        } else if(instr == &i_dint) {
            state->flags_values |= STATIC_INT_EN;
            state->cpu.int_en = false;
        } else if(instr == &i_rdi) {
            state->cpu.acc = code->rom[pc+1];
            state->flags_values |= STATIC_ACC;
        } else if(instr == &i_rd) {
            moveReg(state, opcode & i_rd.opcode_mask, REG_ACC);
        } else if(instr == &i_wr) {
            moveReg(state, REG_ACC, opcode & i_wr.opcode_mask);
        } else if(instr == &i_sw) {
            swapReg(state, opcode & i_sw.opcode_mask, REG_ACC);
        }
        // TODO: basically every instruction deletes ACC

        if(!(state->flags_values & STATIC_SP)) {
            if(state->flags_values & STATIC_INT_EN) {
                if(state->cpu.int_en) {
                    state->results |= SP_INT_CHECK_FAIL;
                }
            } else {
                state->results |= SP_INT_CHECK_WARN;
            }
        }
        // } else if(isRegWr(opcode, REG_SP)) {
        //     if((state->flags_conflicts & STATIC_INT_EN)) {
        //         state->results |= SP_INT_CHECK_CONFLICT;
        //     } if((state->flags_values & STATIC_INT_EN)) {
        //         if(state->cpu.int_en) {
        //             state->results |= SP_INT_CHECK_FAIL;
        //         }
        //     } else {
        //         state->results |= SP_INT_CHECK_WARN;
        //     }
        // }
        
        i += nbytes(peek(&state->cpu, pc));
    }
}

static void mergeStaticStates(static_state_t* child, static_state_t* parent) {
    if(parent == child) return;
    
    static_param_type_t possible_conflicts =
            parent->flags_values & child->flags_values;
    child->flags_conflicts = parent->flags_conflicts;
    if((possible_conflicts & STATIC_INT_EN)
        && (parent->cpu.int_en != child->cpu.int_en)) {
        child->flags_conflicts |= STATIC_INT_EN;     
    }
    
    child->flags_values = parent->flags_values;
    memcpy(&child->cpu, &parent->cpu, sizeof(cpu_state_t));
}

static void staticCheckHelper(asm_state_t* code, static_state_t* states, size_t idx, size_t parent_idx) {
    static_state_t* state = &states[idx];
    
    if(!idx || state->iters >= MAX_CHECK_ITERS) return;
    state->iters++;
    
    mergeStaticStates(&states[idx], &states[parent_idx]);
    
    staticCheckBlock(code, state);
    staticCheckHelper(code, states, state->block->next_idx, idx);
    staticCheckHelper(code, states, state->block->branch_idx, idx);
}

int staticCheck(asm_state_t* code) {
    #if DEBUG_PRINT_CTRL_GRAPH
        debugPrintCtrlGraph(code);
    #endif
    
    bool failed = false;
    
    ctrl_graph_t* graph = &code->ctrl_graph;
    static_state_t* states = malloc(graph->block_sz * sizeof(static_state_t));
    memset(states, 0, graph->block_sz * sizeof(static_state_t));
    
    memcpy(states[0].cpu.rom, code->rom, sizeof(code->rom));
    for(size_t i = 1; i < graph->block_sz; i++) {
        states[i].block = &graph->blocks[i];
    }
    
    for(size_t i = 1; i < graph->block_sz; i++) {
        // Reset iter counts
        for(size_t i = 1; i < graph->block_sz; i++) {
            states[i].iters = 0;
        }
        
        // Interrupts are disabled at the entry point and ISR
        if(graph->blocks[i].block_pc == 0 || graph->blocks[i].block_pc == ISR_ADDR) {
            states[0].flags_values |= STATIC_INT_EN;
            states[0].cpu.int_en = false;
        } else {
            states[0].flags_values = 0;
        }
        states[0].flags_conflicts = 0;
        
        // If the reference count is 0, this is an entry point or a call we
        // can't analyze, so it should be treated as a fresh start; otherwise,
        // it's reachable, so we can analyze it starting from other blocks
        if(states[i].block->refcount == 0) staticCheckHelper(code, states, i, 0);
    }
    
    for(size_t i = 1; i < graph->block_sz; i++) {
        if(states[i].results & SP_INT_CHECK_CONFLICT) {
            // currently not enabled
            printf("Error in block %ld: SP write accessible with inconsistent interrupt status!\n", i);
        } else if(states[i].results & SP_INT_CHECK_FAIL) {
            printf("Error in block %ld: Interrupts cannot be enabled while value of SP is unknown\n", i);
            failed = true;
        } else if (states[i].results & SP_INT_CHECK_WARN) {
           printf("Warning in block %ld: Cannot determine whether interrupts are enabled\n", i);
        }
    }
    
    free(states);
    
    if(failed)
        return -8;
    
    return 0;
}