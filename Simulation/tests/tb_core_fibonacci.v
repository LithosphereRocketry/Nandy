`timescale 1ns/1ps

`define DELAY 500 // 1Mhz clock

module tb_core_fibonacci();
    reg clk;
    reg rst;
    wire [7:0] result;

    assert #("Incorrect value for result") adone(.value(result === 233));

    // This program is completely non-interactive, so we just set all the
    // interactive elements to 0 or ignore them
    core #("testcode/fibonacci.txt") testGate(
        .int(1'b0),
        .reset(rst),
        .clock(clk),
        .ioin(8'd12),
        .ioout(result)
    );

    initial begin
        $dumpfile(`WAVEPATH);
        $dumpvars;
        // Reset the CPU
        clk = 0;
        rst = 0;
        #(2*`DELAY);
        rst = 1;
        repeat(203) begin
            clk = 1;
            #(`DELAY);
            clk = 0;
            #(`DELAY);
        end
        adone.test();
    end
endmodule