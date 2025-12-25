`timescale 1ns/1ps

/*
3-input and gate
*/

module and3 (
        input a,
        input b,
        input c,
        output q
    );
    
    wire nq;
    nand10 gate(
        .a(a),
        .b(b),
        .c(c),
        .q(nq)
    );

    invert inv(
        .a(nq),
        .q(q)
    );
endmodule