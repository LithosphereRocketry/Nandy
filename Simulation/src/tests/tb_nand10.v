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

    initial begin
        inp = 0;
        #50;
        repeat (8) begin
            inp = inp + 1;
            #50;
        end
        $display("done");
    end

endmodule