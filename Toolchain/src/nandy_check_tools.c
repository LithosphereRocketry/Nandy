#include <string.h>

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

void addNextCtrlBlock(ctrl_graph_t* graph, addr_t pc, int is_linked) {
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

static int floatingStaticCheck(static_state_t* state, size_t block_idx) {
    return 0;
}

int staticCheck(asm_state_t* code) {
    #if DEBUG_PRINT_CTRL_GRAPH
        debugPrintCtrlGraph(code);
    #endif
    
    ctrl_graph_t* graph = &code->ctrl_graph;
    static_state_t* states = malloc(graph->block_sz * sizeof(static_state_t));
    for(size_t i = 1; i < graph->block_sz; i++) {
        states[i] = (static_state_t){0};
        states[i].code = code;
        
        // Interrupts are enabled at the entry point
        if(graph->blocks[i].block_pc == 0) {
            states[i].flags |= STATIC_INT_EN_VAL;
            states[i].cpu.int_en = 1;
        }
        
        if(!graph->blocks[i].refcount) {
            int status = floatingStaticCheck(&states[i], i);
            if(status) return status;
        }
    }
    
    return 0;
}