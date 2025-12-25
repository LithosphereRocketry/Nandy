module mux4(
        input [7:0] a,
        input [7:0] b,
        input [7:0] c,
        input [7:0] d,
        input [1:0] sel,
        output [7:0] q
    );

    assign q = {d, c, b, a}[8*sel +: 8];
endmodule