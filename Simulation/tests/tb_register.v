`timescale 1ns/1ps

module tb_register();
    reg [7:0] din;
    wire [7:0] dout;
    reg en, clk, rst;

    register testGate(
        .d(din),
        .clk(clk),
        .en(en),
        .nclr(rst),
        .q(dout)
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