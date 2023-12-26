`timescale 1ns/1ps

/*
Combinational control logic for the whole CPU

This module is big and ugly. Sorry.
*/

module control(
        input [7:0] inst,
        input cycle,
        input carry,

        output M,
        output S,
        output J,
        output LJ,
        output CLI,
        output LJR,
        output MW,
        output MC,
        output RD,
        output WR,
        output Y,
        output [1:0] RS,
        output WA,
        output ISP,
        output WC,
        output [3:0] ALU,
        output [7:0] SIG
    );

    wire ncycle, nMC;
    nand00 invcycle(
        .a(cycle),
        .b(1'b1),
        .q(ncycle)
    );

    assign RS = inst[1:0];
    assign ALU = inst[3:0];

endmodule