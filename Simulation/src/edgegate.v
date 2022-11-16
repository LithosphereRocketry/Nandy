`timescale 1ns/1ps

/*
Positive-edge gate

Only allows positive edges to occur if enable input is high

Sources:
Basedd on D flip-flop by Wikipedia user Nolanjshettle:
https://en.wikipedia.org/wiki/Flip-flop_(electronics)#/media/File:Edge_triggered_D_flip_flop.svg
*/

module edgegate(
        input clk,
        input en,
        output clkout
    );

    wire dl_q;
    wire dl_nq;
    wire ndl_q;
    wire ndl_nq;
    wire nq;

    nand00 g_din(
        .a(dl_q),
        .b(en),
        .q(dl_nq)
    );
    nand10 g_tri(
        .a(ndl_nq),
        .b(clk),
        .c(dl_nq),
        .q(dl_q)
    );
    nand00 g_nd(
        .a(dl_nq),
        .b(ndl_nq),
        .q(ndl_q)
    );
    nand00 g_ndn(
        .a(ndl_q),
        .b(clk),
        .q(ndl_nq)
    );
    nand00 g_q(
        .a(ndl_nq),
        .b(nq),
        .q(clkout)
    );
    nand00 g_nq(
        .a(clkout),
        .b(clk),
        .q(nq)
    );

endmodule