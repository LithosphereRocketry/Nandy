`timescale 1ns/1ps

/*
74x00 NAND gate (one gate of 4)

Sources:
Toshiba Semiconductor: https://media.digikey.com/pdf/Data%20Sheets/Toshiba%20PDFs/TC74AC00P,%20F,%20FN,%20FS.pdf

Timings:		rise		fall
TC74AC00PF		12.9ns		8.0ns
*/

module nand00(
		input a,
		input b,
		output q
	);

	parameter RISE_TIME = 12.9,
			  FALL_TIME = 8;
	
	assign #(RISE_TIME, FALL_TIME) q = ~(a & b);
endmodule