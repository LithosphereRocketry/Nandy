`timescale 1ns/1ps

module #(parameter rompath = "memory.txt") memory(
        input [15:0] addr,
        input [7:0] din,
        input we,
        input clk,
        output [7:0] dout
    );

    wire [7:0] romout, ramout;

    rom grom(
        .address(addr[14:0]),
        .data(romout)
    );

    ramblock gram(
        .address(addr[14:0]),
        .clk(clk),
        .wr(we),
        .din(din),
        .dout(ramout)
    );

    mux #(8) gmux(
        .a(romout),
        .b(ramout),
        .s(addr[15]),
        .q(dout)
    );
endmodule