module core(
        input clk,

        input [7:0] io_in,
        output [7:0] io_addr,
        output [7:0] io_out
    );

    wire wr_acc, wr_ph, wr_pl, wr_sp, wr_x, wr_y;

    wire [2:0] regsel;

    reg [7:0] acc, x, y, sp;

    assign io_addr = y;

    wire [15:0] p_in;
    wire [15:0] p, q;

    wire [15:0] pointer = regsel[0] ? q : p;
    wire [7:0] iosp = regsel[0] ? io : sp;
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

    wire [7:0] alu_out;

    always @(posedge clk) begin
        if(wr_acc) acc <= alu_out;
        if(wr_sp) sp <= acc;
        if(wr_x) x <= acc;
        if(wr_y) y <= acc;
    end
endmodule