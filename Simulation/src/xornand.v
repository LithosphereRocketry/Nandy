`timescale 1ns/1ps

/*
2-input, 1-bit combination xor/nand gate

Truth table:
    a   b   q   n
    0   0   0   1
    1   0   1   1
    0   1   1   1
    1   1   0   0
*/

module xornand(
        input a,
        input b,
        output q,
        output n
    );
    
    wire na, nb;
    
    nand00 gatenand(
        .a(a),
        .b(b),
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