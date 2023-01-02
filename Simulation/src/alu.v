`timescale 1ns/1ps

/*
ALU
*/

module alu(
        input [7:0] a, b,
        input [3:0] op,
        input cin,
        output [7:0] q,
        output cout
    );
    wire addc, bitc, shiftc, addbitc;
    wire [7:0] add, bitw, shift, addbit;

    addsub g_adder(
        .a(a),
        .b(b),
        .isSub(op[1]),
        .isCarry(op[0]),
        .cin(cin),

        .q(add),
        .cout(addc)
    );

    bitwise g_bitwise (
        .a(a),
        .b(b),
        .op(op[1:0]),
        .cout(bitc),
        .q(bitw)
    );

    shift g_shift(
        .in(a),
        .cin(cin),
        .mode(op[2:0]),
        .out(shift),
        .cout(shiftc)
    );

    mux #(.WIDTH(9)) m_addbit(
        .a({addc, add}),
        .b({bitc, bitw}),
        .s(op[2]),
        .q({addbitc, addbit})
    );

    mux #(.WIDTH(9)) m_out(
        .a({addbitc, addbit}),
        .b({shiftc, shift}),
        .s(op[3]),
        .q({cout, q})
    );

endmodule