module compare(
        input [1:0] aluop,
        input inv,

        input cin,
        input [7:0] a,

        output q
    );

    wire nzero = |a;
    wire sign = a[7];

    wire[3:0] cmp_results = {sign, nzero, cin, 1'b0};
    assign q = inv ^ cmp_results[aluop];

endmodule