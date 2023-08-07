`timescale 1ns/1ps

module tb_adder8();
    reg [7:0] a;
    reg [7:0] b;
    reg cin;
    wire [7:0] q;
    wire cout;

    adder8 testGate(
        .a(a),
        .b(b),
        .cin(cin),
        .cout(cout),
        .q(q)
    );

    initial begin
        a = 255;
        b = 0;
        cin = 1;
        #1000;
    end
endmodule