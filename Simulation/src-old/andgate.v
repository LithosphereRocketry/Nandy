`timescale 1ns/1ps

/*
2-input and gate
*/

module andgate (
        input a,
        input b,
        output q
    );
    
    wire nq;
    nand00 gate(
        .a(a),
        .b(b),
        .q(nq)
    );

    invert inv(
        .a(nq),
        .q(q)
    );
endmodule