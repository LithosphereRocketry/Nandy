module tb_orxor();
    wire a, b, s, q;
    wire realq;
    assign realq = s ? a ^ b : a | b;

    orxor testGate(
        .a(a),
        .b(b),
        .s(s),
        .q(q)
    );

    combitest #("orxor", 3, 1, 35) tester(
        .comp_in({a, b, s}),
        .verify(realq),
        .comp_out(q)
    );
endmodule