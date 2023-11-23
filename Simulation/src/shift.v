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

    wire nenc, encarry, ra, newbit;
    nand00 g_encarry(
        .a(cin),
        .b(mode[0]),
        .q(nenc)
    );
    nand00 g_nenc(
        .a(nenc),
        .b(1'b1),
        .q(encarry)
    );
    mux m_ra(
        .a(in[0]),
        .b(in[7]),
        .s(mode[0]),
        .q(ra)
    );
    mux m_newbit(
        .a(encarry),
        .b(ra),
        .s(mode[1]),
        .q(newbit)
    );
    mux #(.WIDTH(9)) m_out(
        .a({in[7:0], newbit}),
        .b({in[0], newbit, in[7:1]}),
        .s(mode[2]),
        .q({cout, out})
    );
endmodule