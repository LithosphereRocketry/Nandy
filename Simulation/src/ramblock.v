`timescale 1ns/1ps

/*
RAM block that gets rid of tristate logic in memory reads, because tristate is
bad and I don't like it; also makes the RAM positive-edge triggered like the
rest of the CPU
*/

module ramblock (
        input [14:0] address,
        input clk,
        input [7:0] din,
        output [7:0] dout
    );
    
    wire wrtick;

    wire [7:0] bus;

    ram r(
        .address(address),
        .write(wrtick),
        .data(bus)
    );
endmodule