`timescale 1ns/1ps

module tb_dff();
    reg inp;
    reg clk;
    wire q;

    dff testGate(
        .clk(clk),
        .d(inp),
        .q(q)
    );

    initial begin
        inp = 0;
        clk = 0;
        #50;
        clk = 1;
        #50;
        clk = 0;
        inp = 1;
        #50;
        clk = 1;
        #50;
        clk = 0;
        #50;
        clk = 1;
        #50;
        clk = 0;
        inp = 0;
        #50;
        clk = 1;
        #50;
        $display("done");
    end

endmodule