`timescale 1ns/1ps

/*
1-bit, 2-input full adder
*/

module fulladder(
        input a,
        input b,
        input cin,
        output q,
        output cout
    );

    wire qpre, c1, c2;

    xornand xor1(
        .a(a),
        .b(b),
        .q(qpre),
        .n(c1)
    );
    xornand xor2(
        .a(qpre),
        .b(cin),
        .q(q),
        .n(c2)
    );
    nand00 nandc(
        .a(c1),
        .b(c2),
        .q(cout)
    );
endmodule