`timescale 1ns/1ps

module core #(parameter rompath = "memory.txt") (
        input int,
        input reset,
        input clock,
        input [7:0] ioin,
        output [7:0] ioout,
        output iowrite,
        output ioread
    );

    register #(16) pc(
        .d(16'hxxxx),
        .clk(clock),
        .en(1'bx),
        .nclr(reset)
        // output [WIDTH-1:0] q,
        // output [WIDTH-1:0] nq
    );

endmodule