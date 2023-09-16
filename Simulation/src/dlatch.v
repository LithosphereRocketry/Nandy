`timescale 1ns/1ps

/*
! Not yet tested

Basic D latch
Used as the main component for the new pulsegen-based registers.

*/

module dlatch(
        input clk,
        input d,
        output q
    );

    wire diag;
    wire bottom;
    wire nq;
    nand00 g_inp(
        .a(d),
        .b(clk),
        .q(diag)
    );
    nand00 g_low(
        .a(diag),
        .b(clk),
        .q(bottom)
    );
    nand00 l_high(
        .a(diag),
        .b(nq),
        .q(q)
    );
    nand00 l_low(
        .a(q),
        .b(bottom),
        .q(nq)
    );
endmodule