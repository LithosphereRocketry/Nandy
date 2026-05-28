module mux4(
        input [7:0] a,
        input [7:0] b,
        input [7:0] c,
        input [7:0] d,
        input [1:0] sel,
        output [7:0] q
    );

    wire [31:0] inputs = {d, c, b, a};
    assign q = inputs[8*sel +: 8];
endmodule