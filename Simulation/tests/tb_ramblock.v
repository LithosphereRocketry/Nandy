`timescale 1ns/1ps

// Heavily based on tb_register

module tb_ramblock();
    reg [7:0] din;
    wire [7:0] dout;
    reg en, clk, rst;

    // We test with fixed address but really we should check for address
    // changing, idk
    ramblock testGate(
        .address(15'h1234),
        .clk(clk),
        .wr(en),
        .din(din),
        .dout(dout)
    );

    assert #("Value unchanged when it should be written")
        a_unch(.value(dout == 8'h00));
    assert #("Value written when it should have been left alone")
        a_updt(.value(dout == 8'h12));

    initial begin
        $dumpfile(`WAVEPATH);
        $dumpvars;

        // Async reset
        din = 8'h12;
        en = 1'b0;
        clk = 1'b0;
        rst = 1'b1;
        #100;
        a_unch.test();

        // Cycle the clock and make sure it doesn't read
        #100;
        clk = 1'b1;
        #100;
        a_unch.test();
        clk = 1'b0;
        #100;
        a_unch.test();

        // Ok now you can read
        en = 1'b1;
        clk = 1'b1;
        #100;
        a_updt.test();
        // Make sure it only updates on positive edge
        din = 8'h34;
        #100;
        a_updt.test();
        clk = 1'b0;
        #100;
        a_updt.test();

        $finish;
    end
endmodule