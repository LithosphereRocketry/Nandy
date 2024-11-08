#ifndef NANDY_CHECK_TOOLS_H
#define NANDY_CHECK_TOOLS_H

#include "nandy_tools.h"

void updateNextCtrlLink(ctrl_graph_t* graph, addr_t end_pc, size_t next_idx);
void addFloatingCtrlBlock(ctrl_graph_t* graph, addr_t pc);
void addNextCtrlBlock(ctrl_graph_t* graph, addr_t pc, bool is_linked);
void addBranchCtrlBlock(ctrl_graph_t* graph, addr_t origin_pc, addr_t target_pc);

int staticCheck(asm_state_t* code);

#define DEBUG_PRINT_CTRL_GRAPH 1
#if DEBUG_PRINT_CTRL_GRAPH
    void debugPrintCtrlGraph(asm_state_t* state);
#endif

typedef enum ctrl_block_op {
    CTRL_BLOCK_FOUND,
    CTRL_BLOCK_APPENDED,
    CTRL_BLOCK_INSERTED
} ctrl_block_op_t;

typedef struct static_state {
    ctrl_block_t* block;
    
    cpu_state_t cpu;
    enum {
        CTRL_BLOCK_QUEUED = 0x01,
        CTRL_BLOCK_WR_SP  = 0x02,
        STATIC_INT_EN_VAL = 0x04,
    } flags;
    
    bool queued;
} static_state_t;

#endif