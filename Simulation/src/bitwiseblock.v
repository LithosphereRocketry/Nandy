`timescale 1ns/1ps

/*
Multi-function bitwise logic block

op1     op0     q       cout
0       0       0       a!=b
0       1       xor     a!=b*
1       0       and     a>b
1       1       or      a>b*

* inverted at a later stage

*/

module bitwiseblock(
        input op1,
        input op0,
        input a,
        input b,
        input cin,
        output q,
        output cout
    );

    wire qnand, le, ge, ne, qand, andsel, xorsel, bsel, ncin, comp;
    nand00 gnand(
        .a(a),
        .b(b),
        .q(qnand)
    );
    nand00 gle(
        .a(a),
        .b(qnand),
        .q(le)
    );
    nand00 gge(
        .a(qnand),
        .b(b),
        .q(ge)
    );
    nand00 gne(
        .a(ge),
        .b(le),
        .q(ne)
    );
    nand00 gand(
        .a(qnand),
        .b(1),
        .q(qand)
    );
    nand00 gandsel(
        .a(op1),
        .b(qand),
        .q(andsel)
    );
    nand00 gxorsel(
        .a(ne),
        .b(op0),
        .q(xorsel)
    );
    nand00 gq(
        .a(xorsel),
        .b(andsel),
        .q(q)
    );
    nand00 gasel(
        .a(op1),
        .b(b),
        .q(bsel)
    );
    nand00 gncin(
        .a(cin),
        .b(1),
        .q(ncin)
    );
    nand00 gcomp(
        .a(bsel),
        .b(ne),
        .q(comp)
    );
    nand00 gcout(
        .a(ncin),
        .b(comp),
        .q(cout)
    );
endmodule