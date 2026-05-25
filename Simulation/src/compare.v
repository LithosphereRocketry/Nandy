module compare(
        input [1:0] aluop;
        input inv;

        input cin;
        input [7:0] a;

        output q;
    );

    wire nzero = |a;
    wire sign = a[7];

    assign q = inv ^ ({0, cin, nzero, sign}[aluop]);

endmodule