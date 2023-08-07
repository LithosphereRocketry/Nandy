`timescale 1ns/1ps

module tb_nand00();
    `include "debug.vh"

    reg a, b;
    wire q;

    nand00 testGate(
        .a(a),
        .b(b),
        .q(q)
    );

    initial begin
        a = 1'b0;
        b = 1'b0;
        
        assert #(message = "Heehee hoohoo") (1'b0);

        #50;
        a = 1'b1;
        #50;
        b = 1'b1;
        #50;
        a = 1'b0;
        #50;
        b = 1'b0;
        #50;
        a = 1'bx;
        #50;
        b = 1'bx;
        #50;
        a = 1'b1;
        #50;
    end

endmodule