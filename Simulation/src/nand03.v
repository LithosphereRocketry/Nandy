`timescale 1ns/1ps

/*
74x03 open-collector NAND gate (one gate of 4)

Sources:
https://www.ti.com/lit/ds/symlink/sn74hc03.pdf
Timings:		rise		fall
SN74HC03		13ns*		10ns
*/

module nand03(
		input a,
		input b,
		output q
	);

	parameter RISE_TIME = 113, // * includes 100ns delay from pullup RC circuit
			  FALL_TIME = 10;
	
	assign #(RISE_TIME, FALL_TIME) q = ~(a & b);
endmodule