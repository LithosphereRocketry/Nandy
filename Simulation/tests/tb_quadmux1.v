module tb_quadmux1();
    wire a, b, c, d, q;
    wire [1:0] s;
    wire realq;
    assign realq = s == 2'h0 ? a
                 : s == 2'h1 ? b
                 : s == 2'h2 ? c
                 : d;

    quadmux #(1) testGate(
        .a(a),
        .b(b),
        .c(c),
        .d(d),
        .s(s),
        .q(q)
    );

    combitest #("mux1", 6, 1, 57) tester(
        .comp_in({a, b, c, d, s}),
        .verify(realq),
        .comp_out(q)
    );
endmodule