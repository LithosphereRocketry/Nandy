`timescale 1ns/1ps

/*
RAM block that gets rid of tristate logic in memory reads, because tristate is
bad and I don't like it; also makes the RAM positive-edge triggered like the
rest of the CPU
*/

module ramblock (
        input [14:0] address,
        input clk,
        input wr,
        input [7:0] din,
        output [7:0] dout
    );
    
    edgegate #(3) eg(
        .clk(clk),
        .en(wr),
        .out(wrtick)
    );

    wire wrtick;

    wire [7:0] nbus;
    pullup p [7:0] (nbus);

    ram r(
        .address(address),
        .write(wrtick),
        .data(nbus)
    );

    invert invout [7:0] (
        .a(nbus),
        .q(dout)
    );

    wire ntick;
    invert invtick(
        .a(wrtick),
        .q(ntick)
    );
    nand03 gdin [7:0] (
        .a(din),
        .b(ntick),
        .q(nbus)
    );
endmodule