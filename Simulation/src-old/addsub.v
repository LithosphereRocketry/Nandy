`timescale 1ns/1ps

/*
2-input, 8-bit adder/subtractor

Total latency <= 237.9 ns

Truth table:
    isSub   isCarry cin     q
    0       0       x       a + b
    0       1       0       a + b
    0       1       1       a + b + 1
    1       0       x       a - b
    1       1       0       a - b - 1
    1       1       1       a - b
*/

module addsub(
        input [7:0] a,
        input [7:0] b,
        input isSub,
        input isCarry,
        input cin,

        output [7:0] q,
        output cout
    );

    wire [7:0] invb;
    xornand invert [7:0] (
        .a(b),
        .b(isSub),
        .q(invb)
    );

    wire adderCin;

    mux #(1) cinMux(
        .s(isCarry),
        .a(isSub),
        .b(cin),
        .q(adderCin)
    );

    addern #(8) adder(
        .a(a),
        .b(invb),
        .q(q),
        .cin(adderCin),
        .cout(cout)
    );
endmodule