`timescale 1ns/1ps

module tb_nand00();
    reg a, b;
    wire q;

    nand00 testGate(
        .a(a),
        .b(b),
        .q(q)
    );

    assert #("Output 0 when 1 expected") high (.value(q));
    assert #("Output 1 when 0 expected") low (.value(~q));
    assert #("Ouptut defined when undefined expected") undf (.value(q == 1'bx));

    initial begin
        undf.test();
        a = 1'b0;
        b = 1'b0;
        #50;
        high.test();
        a = 1'b1;
        #50;
        high.test();
        b = 1'b1;
        #50;
        low.test();
        a = 1'b0;
        #50;
        high.test();
        b = 1'b0;
        #50;
        high.test();
        a = 1'bx;
        #50;
        high.test();
        b = 1'bx;
        #50;
        undf.test();
        a = 1'b1;
        #50;
        high.test();
        $display("All tests passed");
        $finish(0);
    end

endmodule