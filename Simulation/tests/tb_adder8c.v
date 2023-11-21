module tb_adder8c();
    wire [7:0] a;
    wire [7:0] b;
    wire cin;
    wire [8:0] ans;
    wire [8:0] realans;
    assign realans = (a + b + cin);

    addern #(8) testGate(
        .a(a),
        .b(b),
        .cin(cin),
        .q(ans[7:0]),
        .cout(ans[8])
    );

    combitest #("adder8c", 17, 9, 210) tester(
        .comp_in({a, b, cin}),
        .verify(realans),
        .comp_out(ans)
    );
endmodule