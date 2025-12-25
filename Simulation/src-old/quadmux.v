`timescale 1ns/1ps

/*
2-input, 1-bit mux

This is slightly less gate-efficient, so use selmux if the signal will be shared with other muxes

Truth table:
    s   q
    0   a
    1   b
    2   c
    3   d
*/

module quadmux #(parameter WIDTH=1) (
        input [WIDTH-1:0] a,
        input [WIDTH-1:0] b,
        input [WIDTH-1:0] c,
        input [WIDTH-1:0] d,
        input [1:0] s,
        output [WIDTH-1:0] q
    );
    
    wire [1:0] ns;
    invert inv [1:0] (
        .a(s),
        .q(ns)
    );

    wire [WIDTH-1:0] bus;
    pullup p [WIDTH-1:0] (bus);
    quadmux_segment #(WIDTH) gsegment [3:0] (
        .inp({a, b, c, d}),
        .s1({ns[0], s[0], ns[0], s[0]}),
        .s2({ns[1], ns[1], s[1], s[1]}),
        .bus(bus)
    );

    invert invout [WIDTH-1:0] (
        .a(bus),
        .q(q)
    );
endmodule

module quadmux_segment #(parameter WIDTH=1) (
        input [WIDTH-1:0] inp,
        input s1, s2,
        output [WIDTH-1:0] bus
    );
    
    wire sel;
    andgate gsel(
        .a(s1),
        .b(s2),
        .q(sel)
    );
    nand03 gtrans [WIDTH-1:0] (
        .a(inp),
        .b(sel),
        .q(bus)
    );
endmodule