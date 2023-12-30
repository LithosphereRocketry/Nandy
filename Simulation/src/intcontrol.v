`timescale 1ns/1ps

module intcontrol(
        input [15:0] pcin,
        input nie,
        input nid,
        input int,
        input nclr,
        input nis,
        input nic,
        input ncycle,
        input clk,
        input rst,

        output [15:0] pcout,
        output ienabled,
        output istatus
    );

    // Positive-edge pulse generation
    // All latches here can use ~2C as an enable input, so we can avoid also
    // piping that into the interrupt logic AND gate to save a gate or two
    wire ntick, tick;
    edgegate #(1) eg(
        .clk(clk),
        .en(ncycle),
        .out(ntick)
    );
    invert invtick(.a(ntick), .q(tick));

    // Interrupt repeat prevention
    // An interrupt can only occur when on the last interrupt tick (aka 
    // non-two-cycle clock), the interrupt wasn't present. This means that
    // slower peripherals can interface with the CPU without fear that their
    // interrupts will be double-registered. It's not entirely trivial to decide
    // in which cases the last-value register should be updated - here I've
    // decided on every clock cycle, regardless of if an interrupt is occuring
    // or not, or whether interrupts are enabled.
    wire notprev, new;
    // Because this doesn't use the reset function, this can actually be
    // implemented with four 74x00s and not 3 + a 74x10 to save a bit of money
    // and PCB space, but it's not worth enumerating here becasue the difference
    // in logic is so miniscule
    dlatch gprev(
        .d(int),
        .en(tick),
        .nclr(1'b1),
        .nq(notprev)
    );
    andgate gnew(
        .a(int),
        .b(notprev),
        .q(new)
    );

    // Interrupt handling logic
    // this is the important one: when all three of these go high, an interrupt
    // happens
    wire nready, nistatus;
    nand10 giready(
        .a(ienabled),
        .b(nistatus),
        .c(new),
        .q(nready)
    );
    // Manual set/reset
    // Because of negative logic this is basically an or gate
    wire intsrset;
    nand00 gmanualset(
        .a(nready),
        .b(nis),
        .q(intsrset)
    );

    wire intsrclr;
    nand00 gmanualclr(
        .a(nclr),
        .b(nic),
        .q(intsrclr)
    );

    srlatch intsr(
        .s(intsrset),
        .r(intsrclr),
        .en(tick),
        .nclr(rst),
        .q(istatus),
        .nq(nistatus)
    );

    // Technically it's probably possible to build the interrupt-jump mux with
    // just ands and ors to save a few gates, but using a mux means it's easier
    // to wrap your brain around and the interrupt vector can be moved later
    mux #(16) mpc(
        .a(16'h7F00),
        .b(pcin),
        .s(nready),
        .q(pcout)
    );

    // Interrupt enable/disable
    wire ie, id;
    invert invens(.a(nie), .q(ie));
    invert invenc(.a(nid), .q(id));
    srlatch ensr(
        .s(ie),
        .r(id),
        .en(tick),
        .nclr(rst),
        .q(ienabled)
    );

endmodule