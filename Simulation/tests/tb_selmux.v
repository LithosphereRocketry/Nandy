`timescale 1ns/1ps

module tb_selmux();
    wire a, b, sa, sb, q;
    wire realq;
    assign realq = (a & sa) | (b & sb);

    selmux testGate(
        .a(a),
        .b(b),
        .sa(sa),
        .sb(sb),
        .q(q)
    );

    combitest #("selmux", 4, 1, 21) tester(
        .comp_in({a, b, sa, sb}),
        .verify(realq),
        .comp_out(q)
    );
endmodule