`timescale 1ns/1ps

/*
2-input, 1-bit mux

This is slightly less gate-efficient, so use selmux if the signal will be shared with other muxes

Truth table:
    s   q
    0   a
    1   b
*/

module mux(
        input a,
        input b,
        input s,
        output q
    );
    
    wire ns;
    nand00 inv(
        .a(s),
        .b(1),
        .q(ns)
    );

    selmux m(
        .a(a),
        .b(b),
        .q(q),
        .sa(ns),
        .sb(s)
    );


endmodule