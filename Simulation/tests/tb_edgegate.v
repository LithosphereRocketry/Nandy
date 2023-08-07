`timescale 1ns/1ps

module tb_edgegate();
    reg clk, en;
    wire clkout, refclkout;

    edgegate testGate(
        .clk(clk),
        .en(en),
        .clkout(clkout),
        .refclkout(refclkout)
    );

    initial begin
        clk = 1;
        repeat(16) begin
            #500;
            clk = !clk;
        end
    end
    initial begin
        en <= 0;
        #1100;
        en <= 1; // blip in clk
        #200;
        en <= 0;
        #1300;
        en <= 1; // blip in !clk
        #200;
        en <= 0;
        #500;
        en <= 1; // up in clk
        #1000;
        en <= 0; // down in clk
        #1500;
        en <= 1; // up in !clk
        #1000;
        en <= 0; // down in clk
        #1200;
    end

endmodule