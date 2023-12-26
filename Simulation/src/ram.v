`timescale 1ns/1ps

/*
32Kx8-bit tri-state RAM, active low write-enable

Note: This simulation assumes that CE and OE are always held low, and that input
and output timings are driven entirely by WE

Sources:
Alliance: https://www.alliancememory.com/wp-content/uploads/pdf/AS6C62256.pdf

This model assumes TAS, TWR, and TDH are zero; this is true for this particular
chip but may not be a valid assumption for all chips of this format.

Also, this model does not check if you change the address faster than TRC/TWC,
because it's sort of unclear what doing so actually does; you can violate this
constraint in ways that sorta seem like they shouldn't cause problems given the
other constraints unless something really screwy is going on inside the chip.
In general, just don't do it I guess.

*/

module ram #(
        parameter TRC = 55,
        parameter TAA = 55,
        parameter TOH = 10,

        parameter TWC = 55,
        parameter TAW = 50,
        parameter TWP = 45,
        parameter TWHZ = 20,
        parameter TOW = 5,
        parameter TDW = 25
    ) (
        input [14:0] address,
        input write,
        inout [7:0] data
    );
    
    reg [7:0] ram [0:((1 << 15) - 1)];

    // Read cycle
    wire [14:0] addr_taa;
    wire [14:0] addr_toh;
    assign #(TOH) addr_toh = address;
    assign #(TAA) addr_taa = address;

    wire [7:0] dout;
    assign dout = (addr_taa == addr_toh) ? ram[addr_taa] : 8'hxx;

    wire hz;
    assign #(TWHZ, TOW) hz = ~write;
    assign data = hz ? 8'hzz : dout;

    // Write cycle
    wire [7:0] data_tdw;
    wire [7:0] data_valid;
    assign #(TDW) data_tdw = data;
    assign data_valid = (data == data_tdw) ? data : 8'hxx;

    wire [14:0] addr_taw;
    wire [14:0] addr_valid;
    assign #(TAW) addr_taw = address;
    assign addr_valid = (addr_taw == address) ? address : 15'hxxxx;

    always @(negedge write) begin
        ram[address] = 8'bxx;
    end

    always @(address) begin
        if(~write) begin
            ram[address] = 8'bxx;
        end
    end

    always @(posedge write) begin
        ram[addr_valid] = data_valid;
    end
endmodule