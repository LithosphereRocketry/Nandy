`timescale 1ns/1ps

module tb_edgegate();
    reg clk;
    reg en;
    wire tick;

    edgegate #(2) testGate(
        .clk(clk),
        .en(en),
        .out(tick)
    );

    assert #("Output low, expected high") a_h(.value(tick));
    assert #("Output high, expected low") a_l(.value(~tick));

    initial begin
        $dumpfile(`WAVEPATH);
        $dumpvars;

        clk = 1'b0;
        en = 1'b0;
        a_l.test();
        #100;
        clk = 1'b1;
        a_l.test();
        #50;
        a_l.test();
        #50;
        a_l.test();
        clk = 1'b0;
        #100;
        a_l.test();
        en = 1'b1;
        #100;
        a_l.test();
        clk = 1'b1;
        a_l.test();
        #50;
        a_h.test();
        #50;
        a_l.test();
        clk = 1'b0;
        #100;
        a_l.test();

        $finish;
    end
endmodule