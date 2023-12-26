`timescale 1ns/1ps

/*
Positive-edge detector

Creates a 3-gate-delay pulse when clk transitions from low to high if en is
set high.
*/

module edgegate(
        input clk,
        input en,
        output out
    );

    wire nclk, delclk, ndelclk, nout;

    nand00 delay [2:0] (
        .a({clk, nclk, delclk}),
        .b(1'b1),
        .q({nclk, delclk, ndelclk})
    );

    nand10 maingate(
        .a(clk),
        .b(ndelclk),
        .c(en),
        .q(nout)
    );

    nand00 inv(
        .a(nout),
        .b(1'b1),
        .q(out)
    );

endmodule