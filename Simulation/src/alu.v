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
        .op(op[1:0]),
        .invc(xy),
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

    wire s_encarry, s_ra, s_newbit;
    andgate g_encarry(
        .a(cin),
        .b(op[0]),
        .q(s_encarry)
    ); // Enable carry-in
    mux m_ra(
        .a(a[0]),
        .b(a[7]),
        .s(op[0]),
        .q(s_ra)
    ); // Rotate or arithmetic
    mux m_newbit(
        .a(s_encarry),
        .b(s_ra),
        .s(op[1]),
        .q(s_newbit)
    ); // Decide which new bit we end up with

    quadmux #(9) outmux(
        .a(bitout),
        .b(addout),
        .c({a, s_newbit}),
        .d({a[0], s_newbit, a[7:1]}),
        .s(op[3:2]),
        .q({cout, q})
    );
endmodule