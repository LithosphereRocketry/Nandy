`timescale 1ns/1ps

/*
Gate-level D flip flop

Sources:
Basedd on schematic by Wikipedia user Nolanjshettle:
https://en.wikipedia.org/wiki/Flip-flop_(electronics)#/media/File:Edge_triggered_D_flip_flop.svg
*/

module dff(
        input clk,
        input d,
        output q
    );

    wire dl_q;
    wire dl_nq;
    wire ndl_q;
    wire ndl_nq;
    wire nq;

    nand00 g_din(
        .a(dl_q),
        .b(d),
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
        .q(q)
    );
    nand00 g_nq(
        .a(q),
        .b(dl_q),
        .q(nq)
    );
endmodule