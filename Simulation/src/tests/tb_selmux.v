`timescale 1ns/1ps

module tb_selmux();
    reg [3:0] inp;
    wire q;

    selmux testGate(
        .a(inp[0]),
        .b(inp[1]),
        .sa(inp[2]),
        .sb(inp[3]),
        .q(q)
    );

    initial begin
        inp = 0;
        #50;
        repeat (16) begin
            inp = inp + 1;
            #50;
        end
        $display("done");
    end

endmodule