module tb_fulladder();
    wire a, b, cin;
    wire [1:0] ans;
    wire [1:0] realans;
    assign realans = (a + b + cin);

    fulladder testGate(
        .a(a),
        .b(b),
        .cin(cin),
        .q(ans[0]),
        .cout(ans[1])
    );

    combitest #("fulladder", 3, 2, 63) tester(
        .comp_in({a, b, cin}),
        .verify(realans),
        .comp_out(ans)
    );
endmodule