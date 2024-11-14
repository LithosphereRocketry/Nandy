#ifndef NANDY_CHECK_TOOLS_H
#define NANDY_CHECK_TOOLS_H

#include "nandy_tools.h"

void updateNextCtrlLink(ctrl_graph_t* graph, addr_t end_pc, size_t next_idx);
void addFloatingCtrlBlock(ctrl_graph_t* graph, addr_t pc);
void addNextCtrlBlock(ctrl_graph_t* graph, addr_t pc, bool is_linked);
void addBranchCtrlBlock(ctrl_graph_t* graph, addr_t origin_pc, addr_t target_pc);

#define MAX_CHECK_ITERS 2
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

typedef enum static_param_type {
    STATIC_INT_EN = 1<<0,
    STATIC_ACC = 1<<1,
    STATIC_SP = 1<<2,
    STATIC_DX = 1<<3,
    STATIC_DY = 1<<4
} static_param_type_t;

typedef struct static_state {
    ctrl_block_t* block;
    
    cpu_state_t cpu;
    
    static_param_type_t flags_values;
    static_param_type_t flags_conflicts;
    
    enum {
        SP_INT_CHECK_FAIL = 0x01,
        SP_INT_CHECK_WARN = 0x02,
        SP_INT_CHECK_CONFLICT = 0x04,
        SP_INT_CHECK_MASK = 0x07,
    } results;
    
    int iters;
} static_state_t;

#endif