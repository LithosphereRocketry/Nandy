`timescale 1ns/1ps

module tb_nand10();
    reg [2:0] inp;
    wire q;

    nand10 testGate(
        .a(inp[0]),
        .b(inp[1]),
        .c(inp[2]),
        .q(q)
    );

    assert #("Incorrect value!") val (.value(q == ~(inp[0] & inp[1] & inp[2])));

    initial begin
        inp = 0;
        #50;
        repeat (8) begin
            $display("Testing value %b", inp);
            inp = inp + 1;
            #50;
            val.test();
        end
        $display("All tests passed");
        $finish(0);
    end

endmodule