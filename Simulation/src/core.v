`timescale 1ns/1ps

module core #(parameter rompath = "memory.txt") (
        input int,
        input reset,
        input clock,
        input [7:0] ioin,
        output [7:0] ioout,
        output iowrite,
        output ioread
    );
    wire [15:0] pc;
    wire nM;
    // Technically this invert can be optimized out
    invert invmem(.a(M), .q(nM));
    register #(16) rpc(
        .d(intnewpc),
        .clk(clock),
        .en(nM),
        .nclr(reset),
        .q(pc)
    );

    wire [15:0] memaddr, ret, newpc;
    addresscalc acalc(
        .pcin(pc),
        .dx(dx),
        .dy(dy),
        .sp(sp),
        .longoffs(memout),
        .offs(instr[3:0]),
        .mem(M),
        .stack(S),
        .jump(J),
        .longjump(LJ),

        .addr(memaddr),
        .ret(ret),
        .pcout(newpc)
    );

    wire [7:0] memout;
    memory #(rompath) mem(
        .addr(memaddr),
        .din(acc),
        .we(MW),
        .clk(clock),
        .dout(memout)
    );

    wire [7:0] instr;
    dlatch il [7:0] (
        .en(ncycle),
        .d(memout),
        .nclr(1'b1),
        .q(instr)
    );

    // There's a few control signals
    wire M, S, J, LJ, nCLI, nLJR, MW, MC, RD, WR, Y, WA, nISP, WC;
    wire [1:0] RS;
    wire [3:0] aluop;
    wire [7:0] nSIG;
    control ctrl(
        .inst(instr),
        .cycle(cycle),
        .ncycle(ncycle),
        .carry(carry),

        .M(M),
        .S(S),
        .J(J),
        .LJ(LJ),
        .nCLI(nCLI),
        .nLJR(nLJR),
        .MW(MW),
        .MC(MC),
        .RD(RD),
        .WR(WR),
        .Y(Y),
        .RS(RS),
        .WA(WA),
        .nISP(nISP),
        .WC(WC),
        .ALU(aluop),
        .nSIG(nSIG)
    );

    wire cycle, ncycle;
    register #(1) rcycle(
        .d(MC),
        .clk(clock),
        .en(1'b1),
        .nclr(reset),
        .q(cycle),
        .nq(ncycle)
    );

    wire [15:0] intnewpc;
    wire ienabled, istatus;
    intcontrol ictrl(
        .pcin(newpc),
        .nie(nSIG[5]),
        .nid(nSIG[4]),
        .int(int),
        .nclr(nCLI),
        .nis(nSIG[7]),
        .nic(nSIG[6]),
        .ncycle(ncycle),
        .clk(clock),
        .rst(reset),

        .pcout(intnewpc),
        .ienabled(ienabled),
        .istatus(istatus)
    );

    wire [7:0] acc, sp, dx, dy, alua, alub;
    regfile rfile(
        .RD(RD),
        .WR(WR),
        .Y(Y),
        .cycle(cycle),
        .RS(RS),
        .WA(WA),
        .nISP(nISP),
        .nLJR(nLJR),
        .clk(clock),
        .RA(ret),
        .ienabled(ienabled),
        .istatus(istatus),
        .intRA(newpc),
        .aluout(aluout),
        .mem(memout),
        .ioin(ioin),
        .imm(instr[3:0]),

        .ior(ioread),
        .iow(iowrite),
        .A(alua),
        .B(alub),

        .acc(acc),
        .sp(sp),
        .dx(dx),
        .dy(dy),
        .ioout(ioout)
    );

    wire carry;
    wire cout;
    wire [7:0] aluout;
    alu ALU(
        .a(alua),
        .b(alub),
        .op(aluop),
        .xy(Y),
        .cin(carry),
        .q(aluout),
        .cout(cout)
    );
    register #(1) rcarry(
        .d(cout),
        .clk(clock),
        .en(WC),
        .nclr(1'b1),
        .q(carry)
    );
endmodule