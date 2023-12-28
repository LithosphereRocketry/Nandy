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

    wire [7:0] ninst;
    invert invinst [7:4] (
        .a(inst[7:4]),
        .q(ninst[7:4])
    );

    and3 gM(
        .a(inst[7]),
        .b(cycle),
        .c(ninst[6]),
        .q(M)
    );

    assign S = inst[4];

    wire jtype, jactive;
    and3 gjtype(
        .a(inst[7]),
        .b(inst[6]),
        .c(inst[5]),
        .q(jtype)
    );
    nand00 gjactive(
        .a(inst[4]),
        .b(carry),
        .q(jactive)
    );
    and3 gJ(
        .a(jtype),
        .b(cycle),
        .c(jactive),
        .q(J)
    );

    wire ncycle;
    invert invcycle(
        .a(cycle),
        .q(ncycle)
    );
    andgate gMC(
        .a(ncycle),
        .b(inst[7]),
        .q(MC)
    );

    assign Y = inst[5];
    assign RS = inst[1:0];
    assign ALU = inst[3:0];

endmodule