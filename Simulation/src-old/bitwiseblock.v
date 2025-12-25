`timescale 1ns/1ps

/*
Multi-function bitwise logic block

op1     op0     q
0       0       b
0       1       A ^ B
1       0       A & B
1       1       A | B
*/

module bitwiseblock(
        input op1,
        input nop1,
        input op0,
        input nop0,
        input a,
        input b,
        output q
    );

    wire n, xa, xb, bot, selb, top;

    nand10 gnand(
        .a(a),
        .b(op1),
        .c(b),
        .q(n)
    );
    nand00 ga(
        .a(a),
        .b(n),
        .q(xa)
    );
    nand00 gb(
        .a(n),
        .b(b),
        .q(xb)
    );
    nand00 gxor(
        .a(xa),
        .b(xb),
        .q(bot)
    );
    nand00 gselb(
        .a(nop1),
        .b(b),
        .q(selb)
    );
    nand00 gand(
        .a(selb),
        .b(n),
        .q(top)
    );
    selmux gmux(
        .a(top),
        .sa(nop0),
        .b(bot),
        .sb(op0),
        .q(q)
    );
endmodule