`timescale 1ns/1ps

/*
Positive-edge detector

Creates a (N*2+1)-gate-delay pulse when clk transitions from low to high if en is
set high.
*/

module edgegate #(parameter REPEATS = 1) (
        input clk,
        input en,
        output out
    );

    wire [REPEATS*2:0] delchain;
    wire nout;

    nand00 delay [REPEATS*2:0] (
        .a({clk, delchain[REPEATS*2:1]}),
        .b(1'b1),
        .q(delchain[REPEATS*2:0])
    );

    nand10 maingate(
        .a(clk),
        .b(delchain[0]),
        .c(en),
        .q(nout)
    );

    nand00 inv(
        .a(nout),
        .b(1'b1),
        .q(out)
    );

endmodule