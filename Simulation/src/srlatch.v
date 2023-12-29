`timescale 1ns/1ps

/*

Active high SR latch

*/

module srlatch(
        input en,
        input s,
        input r,
        input nclr,
        output q,
        output nq
    );

    wire se, re;

    nand00 gse(
        .a(s),
        .b(en),
        .q(se)
    );
    nand00 gre(
        .a(r),
        .b(en),
        .q(re)
    );
    nand00 gh(
        .a(se),
        .b(nq),
        .q(q)
    );
    nand10 gl(
        .a(re),
        .b(q),
        .c(nclr),
        .q(nq)
    );
endmodule 