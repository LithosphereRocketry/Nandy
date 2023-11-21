`timescale 1ns/1ps

/*
256x8-bit ROM


Sources:
Atmel/Microchip: https://media.digikey.com/pdf/Data%20Sheets/Atmel%20PDFs/AT28C16.pdf

Timings:    Tacc
AT28C16     150 ns

*/

module rom(
        input [7:0] address,
        output [7:0] data
    );
    
    parameter ACCESS_TIME = 150;

    reg [7:0] rom [0:255];

    initial begin
        $readmemh("memory.txt", rom);
    end

    assign #(ACCESS_TIME) data = rom[address];

endmodule