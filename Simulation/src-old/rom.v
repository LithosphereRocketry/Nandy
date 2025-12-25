`timescale 1ns/1ps

/*
256x8-bit ROM


Sources:
Atmel/Microchip: https://media.digikey.com/pdf/Data%20Sheets/Atmel%20PDFs/AT28C16.pdf

Timings:    Tacc
AT28C16     150 ns

*/

module rom #(parameter PATH = "memory.txt") (
        input [14:0] address,
        output [7:0] data
    );

    reg [7:0] rom [0:((1 << 15) - 1)];

    initial begin
        $readmemh(PATH, rom);
    end

    assign #(150) data = rom[address];

endmodule