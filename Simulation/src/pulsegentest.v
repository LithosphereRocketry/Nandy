`timescale 1ns/1ps

/*

Test clock divider for new pulsegen circuit

*/

module pulsegentest(
        input clk,
        input nrst,
        output clkout
    );
    
    wire nclk, clkdel, nclkdel, npulse, pulse;
    nand00 gateinp1(
        .a(clk),
        .b(1),
        .q(nclk)
    );
    nand00 gateinp2(
        .a(nclk),
        .b(1),
        .q(clkdel)
    );
    nand00 gateinp3(
        .a(clkdel),
        .b(1),
        .q(nclkdel)
    );


    nand00 gatepgen(
        .a(clk),
        .b(nclkdel),
        .q(npulse)
    );
    nand00 gatepinv(
        .a(npulse),
        .b(1),
        .q(pulse)
    );

    wire latchin, latchout, nlatchout, latchh, latchl;
    nand00 gatelih(
        .a(latchin),
        .b(pulse),
        .q(latchh)
    );
    nand00 gatelil(
        .a(latchh),
        .b(pulse),
        .q(latchl)
    );
    nand00 gatelh(
        .a(latchh),
        .b(nlatchout),
        .q(latchout)
    );
    nand00 gatell(
        .a(latchl),
        .b(latchout),
        .q(nlatchout)
    );
    nand00 gatecdiv(
        .a(latchout),
        .b(nrst),
        .q(latchin)
    );

    assign clkout = latchout;
    

endmodule