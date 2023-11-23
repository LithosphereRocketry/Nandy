`timescale 1ns/1ps

/*
ALU
*/

module alu(
        input [7:0] a, b,
        input [3:0] op,
        input xy,
        input cin,
        output [7:0] q,
        output cout
    );

    wire [8:0] bitout;
    bitwise bit(
        .a(a),
        .b(b),
        .cin(cin),
        .op(op[2:0]),
        .cout(bitout[8]),
        .q(bitout[7:0])
    );

    wire [8:0] addout;
    addsub adder(
        .a(a),
        .b(b),
        .isSub(op[1]),
        .isCarry(op[0]),
        .cin(cin),
        .q(addout[7:0]),
        .cout(addout[8])
    );

    wire [8:0] shiftout;
    shift shifter(
        .in(a),
        .cin(cin),
        .mode({xy, op[1:0]}),
        .out(shiftout[7:0]),
        .cout(shiftout[8])
    );

    wire [8:0] arithout;
    mux #(9) arithmux(
        .a(addout),
        .b(shiftout),
        .s(op[2]),
        .q(arithout)
    );

    mux #(9) outmux(
        .a(bitout),
        .b(arithout),
        .s(op[3]),
        .q({cout, q})
    );
endmodule