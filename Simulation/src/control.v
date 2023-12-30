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
        output nCLI,
        output nLJR,
        output MW,
        output MC,
        output RD,
        output WR,
        output Y,
        output [1:0] RS,
        output WA,
        output nISP,
        output WC,
        output [3:0] ALU,
        output [7:0] nSIG,
        output ncycle // for intcontrol only
    );

    wire [7:0] ninst;
    invert invinst [7:0] (
        .a(inst[7:0]),
        .q(ninst[7:0])
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

    wire simple;
    and3 gsimple(
        .a(ninst[7]),
        .b(ninst[6]),
        .c(ninst[5]),
        .q(simple)
    );

    and3 gLJ(
        .a(simple),
        .b(inst[4]),
        .c(ninst[3]),
        .q(LJ)
    );

    nand00 gCLI(
        .a(LJ),
        .b(inst[1]),
        .q(nCLI)
    );

    nand00 gLJR(
        .a(LJ),
        .b(inst[2]),
        .q(nLJR)
    );

    andgate gMW(
        .a(M),
        .b(inst[5]),
        .q(MW)
    );

    invert invcycle(
        .a(cycle),
        .q(ncycle)
    );
    andgate gMC(
        .a(ncycle),
        .b(inst[7]),
        .q(MC)
    );

    and3 gRD(
        .a(simple),
        .b(ninst[4]),
        .c(inst[2]),
        .q(RD)
    );

    and3 gWR(
        .a(simple),
        .b(ninst[4]),
        .c(inst[3]),
        .q(WR)
    );

    assign Y = inst[5];
    assign RS = inst[1:0];

    // ALL of the garbage that follows is to calculate Writes Accumulator (WA).
    // This is what you get when all of your instructions edit the same register
    wire isalu, nalur, nalui;
    nand00 galur(
        .a(ninst[7]),
        .b(inst[6]),
        .q(nalur)
    );
    nand10 galui(
        .a(cycle),
        .b(inst[6]),
        .c(ninst[5]),
        .q(nalui)
    );
    nand00 galu(
        .a(nalur),
        .b(nalui),
        .q(isalu)
    );
    wire writesacc;
    nand00 gwritesacc(
        .a(inst[4]),
        .b(ninst[3]),
        .q(writesacc)
    );
    wire nwam, nwaa;
    nand00 gwam(
        .a(M),
        .b(ninst[5]),
        .q(nwam)
    );
    nand00 gwaa(
        .a(isalu),
        .b(writesacc),
        .q(nwaa)
    );
    nand00 gWA(
        .a(nwam),
        .b(nwaa),
        .q(WA)
    );
    // FINALLY the end of WA.

    nand10 gnisp(
        .a(ninst[7]),
        .b(ninst[6]),
        .c(inst[5]),
        .q(nISP)
    );

    wire nisalu, carryable;
    invert gnisalu(
        .a(isalu),
        .q(nisalu)
    );
    nand00 gncarryable(
        .a(nisalu),
        .b(nISP),
        .q(carryable)
    );
    andgate gWC(
        .a(carryable),
        .b(inst[4]),
        .q(WC)
    );

    selmux gALU3(
        .a(inst[3]),
        .sa(inst[6]),
        .b(ninst[7]),
        .sb(ninst[6]),
        .q(ALU[3])
    );
    andgate ALUrest [2:0] (
        .a(inst[2:0]),
        .b(inst[6]),
        .q(ALU[2:0])
    );

    wire issig;
    wire [7:0] dec;
    and3 gissig(
        .a(simple),
        .b(inst[4]),
        .c(inst[3]),
        .q(issig)
    );

    and3 gdec0(
        .a(ninst[2]),
        .b(ninst[1]),
        .c(ninst[0]),
        .q(dec[0])
    );
    and3 gdec1(
        .a(ninst[2]),
        .b(ninst[1]),
        .c(inst[0]),
        .q(dec[1])
    );
    and3 gdec2(
        .a(ninst[2]),
        .b(inst[1]),
        .c(ninst[0]),
        .q(dec[2])
    );
    and3 gdec3(
        .a(ninst[2]),
        .b(inst[1]),
        .c(inst[0]),
        .q(dec[3])
    );
    and3 gdec4(
        .a(inst[2]),
        .b(ninst[1]),
        .c(ninst[0]),
        .q(dec[4])
    );
    and3 gdec5(
        .a(inst[2]),
        .b(ninst[1]),
        .c(inst[0]),
        .q(dec[5])
    );
    and3 gdec6(
        .a(inst[2]),
        .b(inst[1]),
        .c(ninst[0]),
        .q(dec[6])
    );
    and3 gdec7(
        .a(inst[2]),
        .b(inst[1]),
        .c(inst[0]),
        .q(dec[7])
    );

    nand00 gSIG [7:0] (
        .a(dec),
        .b(issig),
        .q(nSIG)
    );
endmodule