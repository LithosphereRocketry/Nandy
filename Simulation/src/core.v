/*
Main datapath of NANDy CPU. Denoted in green in block diagram.
*/

module core(
        input clk,

        input [7:0] io_in,
        output [7:0] io_addr,
        output [7:0] io_out
    );

    wire carry;
    wire wr_acc, wr_ph, wr_pl, wr_sp, wr_x, wr_y, wr_mem, alu_from_mem, p_from_addr;

    wire [2:0] regsel;

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

        .use_add(), .do_interrupt() .in_interrupt(), .wr_pc(),
        .base_sel(),

        .immediate(),
        .pq(pointer),
        .sp(sp),

        .addr(mem_addr)
    );

    wire [7:0] mem_out;
    wire [7:0] alu_b = alu_from_mem ? mem_out : reg_read;

    wire [7:0] alu_out;
    alu arith(
        .clk(clk),
        .ncycle(),
        .cmpinv(regsel[0]),
        .aluop(),
        .a(acc),
        .b(alu_b),
        .status(),
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