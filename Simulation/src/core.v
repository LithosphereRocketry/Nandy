/*
Main datapath of NANDy CPU. Denoted in green in block diagram.
*/

module core(
        input clk,

        input [7:0] io_in,
        input [5:0] ints_in,
        output [7:0] io_addr,
        output [7:0] io_out,
        output wr_io
    );

    wire carry, int_en, int_active;
    wire wr_acc, wr_ph, wr_pl, wr_qh, wr_ql, wr_sp, wr_x, wr_y, wr_mem,
        p_from_addr, ncycle, n_addr_use_add, n_do_interrupt, in_interrupt, write_pc;
    wire [1:0] base_sel;
    wire [2:0] aluop;
    wire [2:0] regsel;
    wire [15:0] addr_imm;

    reg [7:0] acc, x, y, sp;

    assign io_addr = y;
    assign io_out = acc;

    reg [15:0] pc, ia;

    wire [15:0] mem_addr;
    wire [15:0] p_in = p_from_addr ? mem_addr : {acc, acc};
    wire [15:0] p, q;

    wire [15:0] pointer = regsel[0] ? q : p;
    wire [7:0] iosp = regsel[0] ? io_in : sp;
    wire [7:0] xy = regsel[0] ? y : x;
    wire [7:0] reg_read;
    mux4 muxreg(
        .a(iosp),
        .b(xy),
        .c(pointer[7:0]),
        .d(pointer[15:8]),
        .sel(regsel[2:1]),
        .q(reg_read)
    );

    split_reg regp(
        .clk(clk),
        .d(p_in),
        .we({wr_ph, wr_pl}),
        .q(p)
    );

    split_reg regq(
        .clk(clk),
        .d({acc, acc}),
        .we({wr_qh, wr_ql}),
        .q(q)
    );

    addr_calc addrcalc (
        .clk(clk),

        .n_use_add(n_addr_use_add), .n_do_interrupt(n_do_interrupt),
        .in_interrupt(in_interrupt), .wr_pc(write_pc),
        .base_sel(base_sel),

        .immediate(addr_imm),
        .pq(pointer),
        .sp(sp),

        .addr(mem_addr)
    );

    wire [7:0] mem_out;
    wire [7:0] alu_b = ncycle ? reg_read : mem_out;

    wire [7:0] alu_out;
    alu arith(
        .clk(clk),
        .ncycle(ncycle),
        .cmpinv(regsel[0]),
        .aluop(aluop),
        .a(acc),
        .b(alu_b),
        .status({int_active, int_en, ints_in}),
        .q(alu_out),
        .carry(carry)
    );

    memory mem(
        .clk(clk),
        .addr(mem_addr),
        .din(acc),
        .wr(wr_mem),
        .dout(mem_out)
    );

    always @(posedge clk) begin
        if(wr_acc) acc <= alu_out;
        if(wr_sp) sp <= acc;
        if(wr_x) x <= acc;
        if(wr_y) y <= acc;
    end
endmodule