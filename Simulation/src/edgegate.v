`timescale 1ns/1ps

module edgegate(
        input clk,
        input en,
        output clkout
    );

    // RS latch
    wire nclkup;
    nand00 engate(
        .a(clk),
        .b(en),
        .q(nclkup)
    );

    wire nclkout;
    nand00 latch_q(
        .a(nclkup),
        .b(nclkout),
        .q(clkout)
    );
    nand00 latch_nq(
        .a(clk),
        .b(clkout),
        .q(nclkout)
    );

endmodule