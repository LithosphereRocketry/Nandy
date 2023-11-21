`timescale 1ns/1ps

/*

Active high D latch

*/

module dlatch(
        input en,
        input d,
        output q
    );

    wire s, r, nq;

    nand00 gd(
        .a(d),
        .b(en),
        .q(s)
    );
    nand00 gen(
        .a(s),
        .b(en),
        .q(r)
    );
    nand00 gh(
        .a(s),
        .b(nq),
        .q(q)
    );
    nand00 gl(
        .a(r),
        .b(q),
        .q(nq)
    );
endmodule 