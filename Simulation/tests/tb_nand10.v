`timescale 1ns/1ps

module tb_nand10();
    wire a, b, c, q;
    wire realq;
    assign realq = ~(a & b & c);

    nand10 testGate(
        .a(a),
        .b(b),
        .c(c),
        .q(q)
    );
    
    combitest #("nand10", 3, 1, 20) tester(
        .comp_in({a, b, c}),
        .verify(realq),
        .comp_out(q)
    );
endmodule