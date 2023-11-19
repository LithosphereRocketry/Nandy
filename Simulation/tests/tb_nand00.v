`timescale 1ns/1ps

module tb_nand00();
    wire a, b, q;
    wire realq;
    assign realq = ~(a & b);

    nand00 testGate(
        .a(a),
        .b(b),
        .q(q)
    );

    combitest #("nand00", 2, 1, 20) tester(
        .comp_in({a, b}),
        .verify(realq),
        .comp_out(q)
    );

endmodule