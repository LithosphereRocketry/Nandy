module tb_mux1();
    wire a, b, s, q;
    wire realq;
    assign realq = s ? b : a;

    mux #(1) testGate(
        .a(a),
        .b(b),
        .s(s),
        .q(q)
    );

    combitest #("mux1", 3, 1, 34) tester(
        .comp_in({a, b, s}),
        .verify(realq),
        .comp_out(q)
    );
endmodule