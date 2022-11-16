`timescale 1ns/1ps

module tb_rom();
    reg [7:0] addr;
    wire [7:0] out;

    rom test(
        .address(addr),
        .data(out)
    );

    initial begin
        addr = 0;
        #1000;
        addr = 5;
        #1000;
        addr = 9;
        #1000;
        addr = 'ha;
        #1000;
        addr = 'h1e;
        #1000;
        addr = 'h2f;
        #1000;
    end
endmodule