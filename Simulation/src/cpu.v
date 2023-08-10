`timescale 1ns/1ps

module cpu #(parameter ROM_PATH = "") (
        input clk,
        input [7:0] ioin,
        output [7:0] ioout
    );

    reg [7:0] acc;
    reg [7:0] dx;
    reg [7:0] dy;
    reg [7:0] sp;
    reg carry;

    wire [7:0] new_acc;
    wire [7:0] new_dx;
    wire [7:0] new_dy;
    wire [7:0] new_sp;
    wire [7:0] new_ioout;
    wire new_carry;

    wire w_acc;
    wire w_dx;
    wire w_dy;
    wire w_sp;
    wire w_ioout;
    wire w_carry;

    always @(posedge clk) begin
        if (w_acc) acc <= new_acc;
        if (w_dx) dx <= new_dx;
        if (w_dy) dy <= new_dy;
        if (w_sp) sp <= new_sp;
        if (w_ioout) ioout <= new_ioout;
        if (w_carry) carry <= new_carry;
    end

endmodule