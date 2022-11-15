`timescale 1ns/1ps

module tb_edgegate();
    reg clk, en;
    wire clkout;

    edgegate testGate(
        .clk(clk),
        .en(en),
        .clkout(clkout)
    );

    initial begin
        clk = 1;
        repeat(12) begin
            #500;
            clk = !clk;
        end
    end
    initial begin
        en <= 0;
        #2100;
        en <= 1;
        #2200;
        en <= 0;
        #100;
        en <= 1;
    end

endmodule