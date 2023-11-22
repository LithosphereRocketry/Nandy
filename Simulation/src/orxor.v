`timescale 1ns/1ps

/*
2-input selectable or/xor gate

Truth table:
    a   b   s   q
    0   0   0   0
    1   0   0   1
    0   1   0   1
    1   1   0   1
    0   0   1   0
    1   0   1   1
    0   1   1   1
    1   1   1   0
*/

module orxor(
        input a,
        input b,
        input s,
        output q
    );
    
    wire na, nb, n;
    
    nand10 gatenand(
        .a(a),
        .b(b),
        .c(s),
        .q(n)
    );
    nand00 gatea(
        .a(a),
        .b(n),
        .q(na)
    );
    nand00 gateb(
        .a(b),
        .b(n),
        .q(nb)
    );
    nand00 gateq(
        .a(na),
        .b(nb),
        .q(q)
    );
endmodule