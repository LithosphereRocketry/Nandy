`timescale 1ns/1ps

/**
Bitwise portion of ALU
*/

module bitwise(
        input [7:0] a, b,
        input [2:0] op,
        input cin,
        output cout,
        output [7:0] q
    );

    wire [1:0] nop;

    nand00 opinv [1:0] (
        .a(op[1:0]),
        .b(1'b1),
        .q(nop)
    );

    wire [8:0] preq;
    bitwiseblock block [7:0] (
        .op1(op[1]),
        .nop1(nop[1]),
        .op0(op[0]),
        .nop0(nop[0]),
        .a(a),
        .b(b),
        .q(preq[7:0])
    );

    wire scarry, zpar;
    selmux gscarry(
        .a(a[7]),
        .sa(nop[0]),
        .b(cin),
        .sb(op[0]),
        .q(scarry)
    );

    zeroparity gzpar(
        .mode(op[0]),
        .data(a),
        .q(zpar)
    );

    selmux gcarry(
        .a(scarry),
        .sa(nop[1]),
        .b(zpar),
        .sb(op[1]),
        .q(preq[8])
    );

    xornand inv [8:0] (
        .a(preq),
        .b(op[2]),
        .q({cout, q})
    );
endmodule