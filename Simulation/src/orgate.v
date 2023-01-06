`timescale 1ns/1ps

module orgate(
        input a,
        input b,
        output q
    );
    wire na, nb;
    nand00 inva(
        .a(a),
        .b(1),
        .q(na)
    );
    nand00 invb(
        .a(b),
        .b(1),
        .q(nb)
    );
    nand00 org(
        .a(na),
        .b(nb),
        .q(q)
    );
endmodule