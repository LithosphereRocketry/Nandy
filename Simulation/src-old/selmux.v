`timescale 1ns/1ps

/*
2-input, 1-bit selection mux (like a mux, but "neither" is a valid input)

Truth table:
    sa  sb  q
    0   0   0
    1   0   a
    0   1   b
    1   1   a | b
*/

module selmux(
        input a,
        input b,
        input sa,
        input sb,
        output q
    );
    
    wire na, nb;

    nand00 gatea(
        .a(a),
        .b(sa),
        .q(na)
    );
    nand00 gateb(
        .a(b),
        .b(sb),
        .q(nb)
    );
    nand00 gateq(
        .a(na),
        .b(nb),
        .q(q)
    );
endmodule