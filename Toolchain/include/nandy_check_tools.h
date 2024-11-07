#ifndef NANDY_CHECK_TOOLS_H
#define NANDY_CHECK_TOOLS_H

#include "nandy_tools.h"

void updateCurrentCtrlBlock(ctrl_graph_t* graph, addr_t end_pc, size_t next_idx);
void addNextCtrlBlock(ctrl_graph_t* graph, addr_t pc);
void addBranchCtrlBlock(ctrl_graph_t* graph, addr_t origin_pc, addr_t target_pc);

int_state_t checkIntState();

typedef enum ctrl_block_op {
    CTRL_BLOCK_FOUND,
    CTRL_BLOCK_APPENDED,
    CTRL_BLOCK_INSERTED
} ctrl_block_op_t;

#endif