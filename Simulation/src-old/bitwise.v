`timescale 1ns/1ps

/**
Bitwise portion of ALU
*/

module bitwise(
        input [7:0] a, b,
        input [1:0] op,
        input invc,
        input cin,
        output cout,
        output [7:0] q
    );

    wire [1:0] nop;

    invert opinv [1:0] (
        .a(op[1:0]),
        .q(nop)
    );

    wire precarry;
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
    andgate gzcarry(
        .a(cin),
        .b(op[0]),
        .q(zcarry)
    );

    zeroparity gzpar(
        .mode(op[0]),
        .data(a),
        .q(zpar)
    );

    mux gcarry(
        .a(zcarry),
        .b(zpar),
        .s(op[1]),
        .q(precarry)
    );

    xornand inv (
        .a(precarry),
        .b(invc),
        .q(cout)
    );
endmodule