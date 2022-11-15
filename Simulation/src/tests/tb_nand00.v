`timescale 1ns/1ps

module tb_nand00();
    reg a, b;
    wire q;

    nand00 testGate(
        .a(a),
        .b(b),
        .q(q)
    );

    initial begin
        a = 0;
        b = 0;
        
        #50;
        a = 1;
        #50;
        b = 1;
        #50;
        a = 0;
        #50;
        b = 0;
        #50;
        a = 'x;
        #50;
        b = 'x;
        #50;
        a = 1;
        #50;
    end

endmodule