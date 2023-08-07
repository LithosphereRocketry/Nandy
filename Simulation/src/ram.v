`timescale 1ns/1ps

/*
256x8-bit tri-state RAM, active low write-enable

Sources:
Renesas: https://www.renesas.com/us/en/document/dst/71256sa-datasheet

Note: Not all timings are simulated here - adding routing delay to the
`data` assign is causing 'x to be written to memory and never fixed
for some reason

*/

module ram(
        input [7:0] address,
        input write,
        inout [7:0] data
    );
    
    parameter TAW = 9;
    parameter TDW = 6;

    reg [7:0] ram [0:255];

    initial begin
        $readmemh("memory.txt", ram);
    end

    wire [7:0] writeAddr;
    wire [7:0] readData;
    assign #(TAW) writeAddr = address;

    assign readData = ram[address];

    always @(*) begin
        if(!write) begin
            ram[writeAddr] = #(TDW) data;   
            $display("Written at %t", $time);
        end
    end
    assign data = (!write) ? 8'bz : readData;


endmodule