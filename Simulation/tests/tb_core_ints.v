`timescale 1ns/1ps

`define DELAY 500 // 1Mhz clock

module tb_core_ints();
    reg clk;
    reg rst;
    reg int;
    reg [7:0] value;
    wire rd;
    wire [7:0] result;
    
    assert #("Incorrect value for result") adone(.value(result === 15));

    // This program is completely non-interactive, so we just set all the
    // interactive elements to 0 or ignore them
    core #("testcode/ints.txt") testGate(
        .int(int),
        .reset(rst),
        .clock(clk),
        .ioin(value),
        .ioread(rd),
        .ioout(result)
    );

    initial begin
        $dumpfile(`WAVEPATH);
        $dumpvars;
        // Reset the CPU
        clk = 0;
        rst = 0;
        int = 0;
        #(2*`DELAY);
        rst = 1;
        repeat(250) begin
            clk = 1;
            #(`DELAY);
            clk = 0;
            #(`DELAY);
        end
        adone.test();
    end

    initial begin
        // Interrupts can come at weird times
        #(22536);
        int = 1;
        value = 1;
        #(52501);
        int = 1;
        value = 2;
        #(10357);
        int = 1;
        value = 3;
        #(90171);
        int = 1;
        value = 4;
        #(45099);
        int = 1;
        value = 5;
    end

    always @(negedge clk) begin
        if(rd) begin
            int = 0;
        end
    end
endmodule