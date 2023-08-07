`timescale 1ns/1ps

module tb_fulladder();
    reg [2:0] inp;
    wire cout, q;

    fulladder testGate(
        .a(inp[0]),
        .b(inp[1]),
        .cin(inp[2]),
        .cout(cout),
        .q(q)
    );

    initial begin
        inp = 0;
        #200;
        repeat (8) begin
            inp = inp + 1;
            #200;
        end
        $display("done");
    end
endmodule