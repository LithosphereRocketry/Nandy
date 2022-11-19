`timescale 1ns/1ps

module tb_addsub();
    reg [7:0] a;
    reg [7:0] b;
    reg cin;
    reg isSub;
    reg isCarry;
    wire [7:0] q;
    wire cout;

    addsub testGate(
        .a(a),
        .b(b),
        .isSub(isSub),
        .isCarry(isCarry),
        .cin(cin),

        .q(q),
        .cout(cout)
    );

    initial begin
        a = 106;
        b = 44;
        cin = 0;
        isSub = 0;
        isCarry = 0;
        #1000;
        isCarry = 1;
        #1000;
        cin = 1;
        #1000;
        isSub = 1;
        #1000;
        cin = 0;
        #1000;
        isCarry = 0;
        #1000;
        a = 0;
        b = 0;
        isSub = 0;
        cin = 0;
        isCarry = 0;
        #1000;
        cin = 1;
        isCarry = 1;
        isSub = 1;
        a = 255;
        b = 255;
        #1000;
    end
endmodule