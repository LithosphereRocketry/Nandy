`timescale 1ns/1ps

/*
Parity/zero flag checker
    if mode = 0, output = (data == 0)
    if mode = 1, output = (popcount(data)%2 == 1)
*/

module zeroparity (
        input mode,
        input [7:0] data,
        output q
    );

    wire [3:0] r1;
    wire [1:0] r2;

    orxor gate [6:0] (
        .a({data[7:4], r1[3:2], r2[1]}),
        .b({data[3:0], r1[1:0], r2[0]}),
        .s(mode),
        .q({r1, r2, q})
    );
endmodule