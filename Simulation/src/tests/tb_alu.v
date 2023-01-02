`timescale 1ns/1ps

module tb_alu();
    reg [7:0] a, b;
    reg [3:0] op;
    reg cin;
    wire [7:0] q;
    wire cout;

    alu test(
        .a(a),
        .b(b),
        .op(op),
        .cin(cin),
        .q(q),
        .cout(cout)
    );

    initial begin
        a = 21;
        b = 7;
        op = 0;
        repeat(16) begin
            cin = 0;
            #1000;
            cin = 1;
            #1000;
            op = op+1;
        end
    end
endmodule