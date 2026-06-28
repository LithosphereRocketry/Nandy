module memory #(parameter PATH = "memory.txt", parameter RAM_DECODED_BITS = 15) (
        input clk,
        input clken,

        input [15:0] addr,
        input [7:0] din,
        input wr,

        output [7:0] dout
    );

    reg [7:0] rom [0:32767];
    reg [7:0] ram [0:(1 << RAM_DECODED_BITS)-1];

    initial $readmemh(PATH, rom);

    assign dout = wr ? 8'hxx :
                  addr[15] ? ram[addr[RAM_DECODED_BITS-1:0]] :
                  rom[addr[14:0]];

    always @(posedge clk) if(clken && wr) ram[addr[RAM_DECODED_BITS-1:0]] <= din;

endmodule