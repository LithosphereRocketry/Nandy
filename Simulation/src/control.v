/*
Control logic for NANDy CPU; denoted in block diagram in blue
*/

module control(
        input clk,
        input clken,
        input rst,

        input [7:0] instr,
        input [5:0] ints_in,
        input carry,

        output wr_acc, wr_ph, wr_pl, wr_qh, wr_ql, wr_sp, wr_x, wr_y, wr_io, wr_mem, wr_carry, 
            p_from_addr, n_addr_use_add, addr_use_imm, n_do_interrupt,  write_pc, status_possible,
        output reg int_en, in_interrupt, ncycle,
        output [1:0] base_sel,
        output [2:0] aluop,
        output [2:0] regsel,
        output [15:0] addr_imm
    );

    reg [7:0] ir;

    initial ncycle = 1;
    wire next_ncycle = ~(ncycle & (instr[6] | instr[7]));

    assign wr_acc = ncycle
        ? (
            instr[7:4] == 4'b0001 // read
            | instr[7:4] == 4'b001 & (~instr[3] | instr[2]) // register ALU
        ) 
        : (
            ir[7] & ir[4] // ld
            | ir[5] & ir[6] // imm alu
        );
    
    assign p_from_addr = ncycle & instr[7:0] == 8'b00000101 // jpr
        | ~ncycle & ir[7:5] == 3'b010 & ir[0] & (~ir[4] | carry) // jr/jcr
        | ~ncycle & ir[7:4] == 4'b1111; // ld +p
    wire [7:0] write_from_move = {8{ncycle & instr[3] & (instr[7:5] == 0)}}
        & (1 << instr[2:0]);
    assign wr_sp = write_from_move[0];
    assign wr_io = write_from_move[1];
    assign wr_x = write_from_move[2];
    assign wr_y = write_from_move[3];
    assign wr_pl = write_from_move[4] | p_from_addr;
    assign wr_ph = write_from_move[5] | p_from_addr;
    assign wr_ql = write_from_move[6];
    assign wr_qh = write_from_move[7];

    assign wr_mem = ~ncycle & ir[7] & ~ir[4];

    assign wr_carry = ncycle & instr[7:5] == 3'b001 & instr[3]
                    | ~ncycle & ir[7:5] == 3'b011 & ir[3];
    
    assign n_addr_use_add = ~(~ncycle & ir[7]);

    assign addr_use_imm = ncycle ? instr[7]
                       : ir[7:5] == 3'b010 & (~ir[4] | carry);

    wire n_jri = ~(ncycle & instr == 8'b00000110);
    assign base_sel = ~ncycle ? 2'b00
        : ~n_jri ? 2'b01
        : (instr[7:5] == 3'b100) ? 2'b11
        : (instr[7] | instr[7:2] == 6'b000001) ? 2'b10
        : 2'b00;
    
    assign aluop = ncycle
        ? (instr[5] ? instr[2:0] : 0)
        : (ir[7] ? 0 : ir[2:0]);

    // assign regsel = ncycle ? (instr[5] ? {2'b01, instr[4]} : instr[2:0])
    //                        : {1'bx, ~ir[6], 1'bx};
    assign regsel = ~ncycle ? 3'bxxx
                  : instr[7] ? {1'bx, ~instr[6], 1'bx}
                  : instr[5] ? {2'b01, instr[4]}
                  : instr[2:0];

    assign addr_imm = ncycle ? {12'b0, instr[3:0]} : {{5{ir[3]}}, ir[3:1], instr};

    wire int_flag = |{ints_in};
    assign n_do_interrupt = ~{int_en & int_flag & ~in_interrupt}; // invert for free

    assign write_pc = ~{ncycle & instr[7]};

    // We can't always read the status register on cycle 1, because register
    // moves also use the ALU B path
    assign status_possible = ncycle & instr[5];

    wire n_keep_interrupt = ~{n_jri & in_interrupt};
    initial in_interrupt = 0;
    wire int_active_next = ~{n_do_interrupt & n_keep_interrupt};

    initial int_en = 0;
    wire int_en_next = int_en | (ncycle & instr == 8'b00000010) & ~(ncycle & instr == 8'b00000011);


    always @(posedge clk) if(rst) begin
        in_interrupt <= 0;
        int_en <= 0;
        ncycle <= 1;
    end else if(clken) begin
        in_interrupt <= int_active_next;
        ncycle <= next_ncycle;
        int_en <= int_en_next;
        ir <= instr;
    end

endmodule