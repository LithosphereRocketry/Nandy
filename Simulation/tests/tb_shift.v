`timescale 1ns/1ps

module tb_shift();
    reg [7:0] a;
    reg cin;
    reg [2:0] mode;
    wire [7:0] q;
    wire cout;

    shift testGate(
        .in(a),
        .cin(cin),
        .mode(mode),
        .out(q),
        .cout(cout)
    );

    initial begin
        a = 8'b10101010;
        mode = 0;
        repeat(8) begin
            cin = 0;
            #100;
            cin = 1;
            #100;
            mode = mode + 1;
        end
    end
endmodule