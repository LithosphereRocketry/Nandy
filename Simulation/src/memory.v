`timescale 1ns/1ps

module memory #(parameter rompath = "memory.txt") (
        input [15:0] addr,
        input [7:0] din,
        input we,
        input clk,
        output [7:0] dout
    );

    wire [7:0] romout, ramout;

    rom #(rompath) grom(
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