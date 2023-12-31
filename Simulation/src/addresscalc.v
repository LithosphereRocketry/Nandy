`timescale 1ns/1ps

module addresscalc(
        input [15:0] pcin,
        input [7:0] dx,
        input [7:0] dy,
        input [7:0] sp,
        input [7:0] longoffs,
        input [3:0] offs,
        input mem,
        input stack,
        input jump,
        input longjump,

        output [15:0] addr,
        output [15:0] ret,
        output [15:0] pcout
    );

    wire [15:0] memaddr;
    mux #(16) mmemaddr(
        .a({dy, dx}),
        .b({8'hFF, sp}),
        .s(stack),
        .q(memaddr)
    );

    wire [15:0] oldaddr;
    mux #(16) moldaddr(
        .a(pcin),
        .b(memaddr),
        .s(mem),
        .q(oldaddr)
    );

    wire [15:0] jumpoffs;
    signext #(12, 16) gjumpoffs(
        .in({offs, longoffs}),
        .out(jumpoffs)
    );

    wire [15:0] memoffs;
    signext #(4, 16) gmemoffs(
        .in(offs),
        .out(memoffs)
    );

    // Program-counter-type offsets
    wire [15:0] ioffs;
    mux #(16) mioffs(
        .a(16'h0001),
        .b(jumpoffs),
        .s(jump),
        .q(ioffs)
    );

    wire [15:0] offset;
    mux #(16) moffset(
        .a(ioffs),
        .b(memoffs),
        .s(mem),
        .q(offset)
    );

    addern #(16) adder(
        .a(oldaddr),
        .b(offset),
        .cin(1'b0),
        .q(ret)
    );

    mux #(16) maddr(
        .a(pcin),
        .b(ret),
        .s(mem),
        .q(addr)
    );

    mux #(16) mpc(
        .a(ret),
        .b({dy, dx}),
        .s(longjump),
        .q(pcout)
    );
endmodule