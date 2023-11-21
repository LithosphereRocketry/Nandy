`timescale 1ns/1ps

/**

*/

module bitwise(
        input [7:0] a, b,
        input [1:0] op,
        output cout,
        output [7:0] q
    );

    wire [6:0] carry;

    bitwiseblock block [7:0] (
        .op1(op[1]),
        .op0(op[0]),
        .a({b[7], a[6:0]}),
        .b({a[7], b[6:0]}),
        .cin({1'b0, carry}),
        .q(q),
        .cout({carry, cout})
    );

endmodule