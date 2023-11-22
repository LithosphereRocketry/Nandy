`timescale 1ns/1ps

/**
Bitwise portion of ALU
*/

module bitwise(
        input [7:0] a, b,
        input [1:0] op,
        input cin,
        output cout,
        output [7:0] q
    );

    wire [1:0] nop;

    nand00 opinv [1:0] (
        .a(op),
        .b(1'b1),
        .q(nop)
    );

    bitwiseblock block [7:0] (
        .op1(op[1]),
        .nop1(nop[1]),
        .op0(op[0]),
        .nop0(nop[0]),
        .a(a),
        .b(b),
        .q(q)
    );

    wire zcarry, zpar;
    selmux gzcarry(
        .a(1'b0),
        .sa(nop[0]),
        .b(cin),
        .sb(op[0]),
        .q(zcarry)
    );

    zeroparity gzpar(
        .mode(op[0]),
        .data(a),
        .q(zpar)
    );

    selmux gcarry(
        .a(zcarry),
        .sa(nop[1]),
        .b(zpar),
        .sb(op[1]),
        .q(cout)
    )
endmodule