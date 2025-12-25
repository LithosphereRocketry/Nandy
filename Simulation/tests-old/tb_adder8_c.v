module tb_adder8_c();
    wire [7:0] a;
    wire [7:0] b;
    wire ans;
    wire [8:0] realans;
    assign realans = (a + b);
    wire realans_c;
    assign realans_c = realans[8];

    addern #(8) testGate(
        .a(a),
        .b(b),
        .cin(1'b0),
        .cout(ans)
    );

    randtest #("adder8_c", 16, 1, 181) tester(
        .comp_in({a, b}),
        .verify(realans_c),
        .comp_out(ans)
    );
endmodule