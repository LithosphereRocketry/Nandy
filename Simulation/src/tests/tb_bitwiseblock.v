`timescale 1ns/1ps

module tb_bitwiseblock();
    reg op1, op0, a, b, cin;
    wire q, cout;

    bitwiseblock test(
        .a(a),
        .b(b),
        .cin(cin),
        .op1(op1),
        .op0(op0),
        .q(q),
        .cout(cout)
    );

    initial begin
        {op1, op0, a, b, cin} = 0;
        repeat(32) begin
            #100;
            {op1, op0, a, b, cin} = {op1, op0, a, b, cin} + 1;
        end
    end
endmodule