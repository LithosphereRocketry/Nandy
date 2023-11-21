module tb_xornand();
    wire a, b, q, n;
    wire realq;
    wire realn;
    assign realq = a ^ b;
    assign realn = (a & b);

    xornand testGate(
        .a(a),
        .b(b),
        .q(q),
        .n(n)
    );

    combitest #("xornand", 2, 2, 35) tester(
        .comp_in({a, b}),
        .verify({realq, realn}),
        .comp_out({q, n})
    );
endmodule