/*
This is weird because it's a hand-optimized collection of NAND gates rather than
a conceptual model like the rest of this sim
*/

module bitwise_slice (
        input op1, nop1, op0, nop0, statsel, nstatsel,
        input a, b, status,
        output q
    );

    wire xor_center = ~(a & b & op1);
    wire xor_left = ~(a & xor_center);
    wire xor_right = ~(b & xor_center);
    wire xor_res = ~(xor_left & xor_right);

    wire b_op1 = ~(b & nop1);
    wire b_and = ~(b_op1 & xor_center);
    wire and_branch = ~(nop0 & b_and);

    wire xor_branch = ~(nstatsel & op0 & xor_res);

    wire stat_branch = ~(statsel & status);

    assign q = ~(and_branch & xor_branch & stat_branch);
endmodule

module bitwise (
        input [1:0] aluop,
        input ncycle,

        input [7:0] a, b, status,

        output [7:0] q
    );

    wire [1:0] naluop = ~aluop;
    wire nstatsel = ~(ncycle & naluop[1] & naluop[0]);
    wire statsel = ~nstatsel;

    bitwise_slice slices [7:0] (
        .op0(aluop[0]),
        .op1(aluop[1]),
        .nop0(naluop[0]),
        .nop1(naluop[1]),
        .statsel(statsel),
        .nstatsel(nstatsel),
        .a(a),
        .b(b),
        .status(status),
        .q(q)
    );

endmodule