`timescale 1ns/1ps

/*

Rising-edge-triggered register with asynchronous clear

*/

module register #(parameter WIDTH = 8) (
        input [WIDTH-1:0] d,
        input clk,
        input en,
        input nclr, 
        output [WIDTH-1:0] q,
        output [WIDTH-1:0] nq
    );  

    wire tick, ntick;

    edgegate edgedet(
        .clk(clk),
        .en(en),
        .out(ntick)
    );

    invert inv(
        .a(ntick),
        .q(tick)
    );

    dlatch latches [WIDTH-1:0] (
        .en(tick),
        .d(d),
        .nclr(nclr),
        .q(q),
        .nq(nq)
    );

endmodule