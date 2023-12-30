/**
WARNING TO FUTURE GENERATIONS

This component is badly compartmentalized and does too many things. This means
that it can be hard to read. Don't be like me and make things like this.
*/

module regfile(
        input RD,
        input WR,
        input Y,
        input cycle,
        input [1:0] RS,
        input WA,
        input nISP,
        input nLJR,
        input clk,
        input [15:0] RA,
        input ienabled,
        input istatus,
        input [15:0] intRA,
        input [7:0] aluout,
        input [7:0] mem,
        input [3:0] imm,

        output ior,
        output iow,
        output [7:0] A,
        output [7:0] B,

        output [7:0] acc,
        output [7:0] sp,
        output [7:0] dx,
        output [7:0] dy,
        output [7:0] ioout
    );

    
endmodule