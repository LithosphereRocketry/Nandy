module tb_bitwiseblock();
    wire op1, op0, a, b;
    wire ans, realans;
    assign realans = op1 ? (op0 ? a^b : a&b) : (op0 ? a|b : b);

    bitwiseblock testGate(
        .op1(op1),
        .nop1(~op1),
        .op0(op0),
        .nop0(~op0),
        .a(a),
        .b(b),
        .q(ans)
    );

    combitest #("bitwiseblock", 4, 1, 53) tester(
        .comp_in({op1, op0, a, b}),
        .verify(realans),
        .comp_out(ans)
    );
endmodule