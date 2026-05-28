module memory #(parameter PATH = "memory.txt") (
        input clk,

        input [15:0] addr,
        input [7:0] din,
        input wr,

        output [7:0] dout
    );

    reg [7:0] rom [0:32767];
    reg [7:0] ram [0:32767];

    initial $readmemh(PATH, rom);

    assign dout = wr ? 8'hxx :
                  addr[15] ? ram[addr[14:0]] :
                  rom[addr[14:0]];

    always @(posedge clk) if(wr) ram[addr[14:0]] <= din;

endmodule