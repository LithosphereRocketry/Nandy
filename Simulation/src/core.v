/*
Main datapath of NANDy CPU. Denoted in green in block diagram.
*/

module core #(parameter PATH = "memory.txt", parameter RAM_DECODED_BITS = 15) (
        input clk,
        input clken,
        input rst,

        input [7:0] io_in,
        input [5:0] ints_in,
        output [7:0] io_addr,
        output [7:0] io_out,
        output wr_io
    );

    wire carry, int_en, int_active;
    wire wr_acc, wr_ph, wr_pl, wr_qh, wr_ql, wr_sp, wr_x, wr_y, wr_mem, wr_carry, addr_use_imm,
        p_from_addr, ncycle, n_addr_use_add, n_do_interrupt, write_pc, status_possible;
    wire [1:0] base_sel;
    wire [2:0] aluop;
    wire [2:0] regsel;
    wire [15:0] addr_imm;

    wire [7:0] mem_out;
    control ctrl(
        .clk(clk),
        .clken(clken),
        .rst(rst),
        .instr(mem_out),
        .ints_in(ints_in),
        .carry(carry),

        .wr_acc(wr_acc),
        .wr_ph(wr_ph),
        .wr_pl(wr_pl),
        .wr_qh(wr_qh),
        .wr_ql(wr_ql),
        .wr_sp(wr_sp),
        .wr_x(wr_x),
        .wr_y(wr_y),
        .wr_io(wr_io),
        .wr_mem(wr_mem),
        .wr_carry(wr_carry),
        .p_from_addr(p_from_addr),
        .n_addr_use_add(n_addr_use_add),
        .addr_use_imm(addr_use_imm),
        .n_do_interrupt(n_do_interrupt),
        .write_pc(write_pc),
        .ncycle(ncycle),
        .status_possible(status_possible),
        .int_en(int_en),
        .in_interrupt(int_active),
        .base_sel(base_sel),
        .aluop(aluop),
        .regsel(regsel),
        .addr_imm(addr_imm)
    );

    reg [7:0] acc, x, y, sp;

    assign io_addr = y;
    assign io_out = acc;

    reg [15:0] pc, ia;

    wire [15:0] mem_addr;
    wire [15:0] p_in = p_from_addr ? mem_addr : {acc, acc};
    wire [15:0] p, q;

    wire [15:0] pointer = regsel[1] ? q : p;
    wire [7:0] spx = regsel[1] ? x : sp;
    wire [7:0] ioy = regsel[1] ? y : io_in;
    wire [7:0] reg_read;
    mux4 muxreg(
        .a(spx),
        .b(ioy),
        .c(pointer[7:0]),
        .d(pointer[15:8]),
        .sel({regsel[2], regsel[0]}),
        .q(reg_read)
    );

    split_reg regp(
        .clk(clk),
        .clken(clken),
        .d(p_in),
        .we({wr_ph, wr_pl}),
        .q(p)
    );

    split_reg regq(
        .clk(clk),
        .clken(clken),
        .d({acc, acc}),
        .we({wr_qh, wr_ql}),
        .q(q)
    );

    addr_calc addrcalc (
        .clk(clk),
        .clken(clken),
        .rst(rst),

        .n_use_add(n_addr_use_add), .n_do_interrupt(n_do_interrupt),
        .in_interrupt(int_active), .wr_pc(write_pc),
        .use_imm(addr_use_imm),
        .base_sel(base_sel),

        .immediate(addr_imm),
        .pq(pointer),
        .sp(sp),

        .addr(mem_addr)
    );

    wire [7:0] alu_b = ncycle ? reg_read : mem_out;

    wire [7:0] alu_out;
    alu arith(
        .clk(clk),
        .clken(clken),
        .wr_carry(wr_carry),
        .status_possible(status_possible),
        .cmpinv(regsel[0]),
        .aluop(aluop),
        .a(acc),
        .b(alu_b),
        .status({int_active, int_en, ints_in}),
        .q(alu_out),
        .carry(carry)
    );

    memory #(PATH, RAM_DECODED_BITS) mem(
        .clk(clk),
        .clken(clken),
        .addr(mem_addr),
        .din(acc),
        .wr(wr_mem),
        .dout(mem_out)
    );

    always @(posedge clk) if(clken) begin
        if(wr_acc) acc <= alu_out;
        if(wr_sp) sp <= acc;
        if(wr_x) x <= acc;
        if(wr_y) y <= acc;
    end
endmodule