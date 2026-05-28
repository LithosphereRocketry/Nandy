/*
ALU for NANDy CPU. Denoted in red in block diagram.
*/

module alu (
        input clk,

        input wr_carry,
        input status_possible, cmpinv,
        input [2:0] aluop,

        input [7:0] a, b, status,

        output [7:0] q,
        output reg carry
    );

    wire [7:0] b_negated = b ^ {8{aluop[0]}};
    wire adder_cin = wr_carry ? carry : aluop[0];
    wire [8:0] sum = a + b_negated + adder_cin;

    wire shift_cin = wr_carry ? carry : 0;
    wire [8:0] q_shleft = {a, shift_cin};
    wire [8:0] q_shright = {a[0], shift_cin, a[7:1]};

    wire [8:0] q_shift = aluop[0] ? q_shright : q_shleft;

    wire [8:0] q_arith = aluop[1] ? q_shift : sum;
    
    wire [7:0] q_bitwise;
    bitwise bw(
        .aluop(aluop[1:0]),
        .status_possible(status_possible),
        .a(a),
        .b(b),
        .status(status),
        .q(q_bitwise)
    );

    wire q_cmp;
    compare cmp(
        .aluop(aluop[1:0]),
        .inv(cmpinv),
        .cin(carry),
        .a(a),
        .q(q_cmp)
    );

    wire [8:0] q_both = aluop[0] ? q_arith : {q_cmp, q_bitwise};
    wire q_carry = q_both[8];
    assign q = q_both[7:0];

    always @(posedge clk) if(wr_carry) carry <= q_carry;


endmodule