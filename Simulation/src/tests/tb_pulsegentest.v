`timescale 1ns/1ps

module tb_pulsegentest();
    reg clkin, rst;
    wire clkout;

    pulsegentest test(
        .clk(clkin),
        .clkout(clkout),
        .nrst(rst)
    );

    initial begin
        clkin = 0;
        rst = 0;
        #100;
        clkin = 1;
        #100;
        clkin = 0;
        #100;
        rst = 1;
        #100;
        repeat(100) begin
            #100;
            clkin = ~clkin;
        end
    end
endmodule