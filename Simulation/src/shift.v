`timescale 1ns/1ps

/*
Shift logic unit
*/

module shift(
        input [7:0] in,
        input cin,
        input [2:0] mode,
        output [7:0] out,
        output cout
    );
    mux #(.WIDTH(9)) m_out(
        .a({in[7:0], newbit}),
        .b({in[0], newbit, in[7:1]}),
        .s(mode[2]),
        .q({cout, out})
    );
endmodule