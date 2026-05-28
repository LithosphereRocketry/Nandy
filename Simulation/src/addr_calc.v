/*
Address calculation datapath for NANDy CPU. Denoted in purple in block diagram.
*/

module addr_calc(
        input clk,

        input n_use_add, use_imm, n_do_interrupt, in_interrupt, wr_pc,
        input [1:0] base_sel,

        input [15:0] immediate,
        input [15:0] pq,
        input [7:0] sp,
        
        output [15:0] addr
    );

    wire [15:0] int_vector = 16'h7F00;

    reg [15:0] pc, ia;
    initial pc = 0;

    wire [15:0] addr_base;
    mux4 muxbase [1:0] (
        .a(pc),
        .b(ia),
        .c(pq),
        .d({7'h7F, in_interrupt, sp}),
        .sel(base_sel),
        .q(addr_base)
    );


    wire [15:0] addr_offset = use_imm ? immediate : 1;

    wire [8:0] lower_sum = addr_base[7:0] + addr_offset[7:0];
    wire cross_carry = (base_sel != 2'b11) & lower_sum[8];
    wire [7:0] upper_sum = addr_base[15:8] + addr_offset[15:8] + cross_carry;
    wire [15:0] addr_sum = {upper_sum, lower_sum[7:0]};

    // With some major rework it might be possible to optimize away this mux
    assign addr = n_use_add ? pc : addr_sum;

    wire [15:0] pc_in = n_do_interrupt ? addr_sum : int_vector;

    always @(posedge clk) begin
        if(~n_do_interrupt) ia <= pc;
        if(wr_pc) pc <= pc_in;
    end

endmodule