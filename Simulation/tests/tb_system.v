`timescale 1ns/1ps

`define DELAY 500 // 1Mhz clock

module tb_system();
    reg clk;
    wire [7:0] io_out;
    wire wr_io;
    reg [7:0] result;

    vassert #("Incorrect value for result") adone(.value(result === 233));

    // This program is completely non-interactive, so we just set all the
    // interactive elements to 0 or ignore them
    core #("obj/hellorld.hex") testGate(
        .clk(clk),
        .clken(1'b1),
        .rst(1'b0),
        .io_in(8'd12),
        .ints_in(6'b000000),
        .io_addr(),
        .io_out(io_out),
        .wr_io(wr_io)
    );

    always @(posedge clk) if(wr_io) result <= io_out;

    initial begin
        $dumpfile(`WAVEPATH);
        $dumpvars;
        // Reset the CPU
        clk = 0;
        #(`DELAY);
        repeat(3200) begin
            #(`DELAY);
            clk = 1;
            #(`DELAY);
            clk = 0;
        end
        adone.test();
    end
endmodule