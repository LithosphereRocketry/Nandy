`timescale 1ns/1ps

module tb_xornand();
    reg [1:0] inp;
    wire q, n;

    xornand testGate(
        .a(inp[0]),
        .b(inp[1]),
        .q(q),
        .n(n)
    );

    initial begin
        inp = 0;
        #50;
        repeat (4) begin
            inp = inp + 1;
            #50;
        end
        $display("done");
    end
endmodule