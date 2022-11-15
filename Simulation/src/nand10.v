`timescale 1ns/1ps

/*
74x10 NAND gate (one gate of 3)

Sources:
Texas Instruments: https://www.ti.com/lit/ds/scas529d/scas529d.pdf

Timings:		rise		fall
SN74AC10        10.5ns      10ns
*/

module nand10(
		input a,
		input b,
        input c,
		output q
	);

	parameter RISE_TIME = 10.5,
			  FALL_TIME = 10;
	
	assign #(RISE_TIME, FALL_TIME) q = ~(a & b & c);
endmodule