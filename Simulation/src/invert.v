`timescale 1ns/1ps

/*
1-bit invert
*/

module invert (
        input a,
        output q
    );
    
    wire ns;
    nand00 inv(
        .a(a),
        .b(1'b1),
        .q(q)
    );
endmodule