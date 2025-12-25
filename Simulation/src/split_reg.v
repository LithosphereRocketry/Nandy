module split_reg(
        input clk;
        input [15:0] d;
        input [1:0] we;
        output reg [15:0] q;
    );

    always @(posedge clk) q = {
        we[1] ? d[15:8] : q[15:8],
        we[0] ? d[7:0] : q[7:0]
    }
endmodule