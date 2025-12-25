`timescale 1ns/1ps
/*
Basic 2-input or gate
*/

module orgate(
        input a,
        input b,
        output q
    );
    wire na, nb;
    invert inva(
        .a(a),
        .q(na)
    );
    invert invb(
        .a(b),
        .q(nb)
    );
    nand00 org(
        .a(na),
        .b(nb),
        .q(q)
    );
endmodule