`timescale 1ns/1ps

/*
This testbench is based on the logisim model rather than "ideal" operation as in
the other testbenches; this is because my test framework doesn't really have a
way to specify "don't care", which would be required for an
implementation-agnostic set of test cases; in other words, most instructions
don't use every signal but the testbench has to be able to verify them
regardless. This isn't incredible TDD practice but ehh whatever.
*/

module tb_control();
    wire [7:0] inst;
    wire cycle;
    wire carry;

    wire M;
    wire S;
    wire J;
    wire LJ;
    wire CLI;
    wire LJR;
    wire MW;
    wire MC;
    wire RD;
    wire WR;
    wire Y;
    wire [1:0] RS;
    wire WA;
    wire ISP;
    wire WC;
    wire [3:0] ALU;
    wire [7:0] SIG;

    wire realM;
    assign realM = inst[7] & ~inst[6] & cycle;

    wire realS;
    assign realS = inst[4];

    wire realJ;
    assign realJ = inst[7] & inst[6] & inst[5] & cycle & ~(carry & inst[4]);

    wire realLJ;
    assign realLJ = ~inst[7] & ~inst[6] & ~inst[5] & inst[4] & ~inst[3];

    wire realCLI;
    assign realCLI = realLJ & inst[1];

    wire realLJR;
    assign realLJR = realLJ & inst[2];

    wire realMW;
    assign realMW = realM & inst[5];

    wire realMC;
    assign realMC = inst[7] & ~cycle;

    wire realRD;
    assign realRD = ~inst[7] & ~inst[6] & ~inst[5] & ~inst[4] & inst[2];

    wire realWR;
    assign realWR = ~inst[7] & ~inst[6] & ~inst[5] & ~inst[4] & inst[3];

    wire realY;
    assign realY = inst[5];
    
    wire [1:0] realRS;
    assign realRS = inst[1:0];

    wire realWA;
    assign realWA = (realM & ~inst[5]) |
        (((inst[6] & ~inst[7]) | (cycle & inst[6] & inst[5])) & ~(inst[4] & ~inst[3]));

    wire realISP;
    assign realISP = ~inst[7] & ~inst[6] & inst[5];

    wire realWC;
    assign realWC = (((inst[6] & ~inst[7]) | (cycle & inst[6] & inst[5]))
                     | realISP) & inst[4];

    wire [3:0] realALU;
    assign realALU = inst[3:0];

    wire [7:0] realSIG;
    assign realSIG = (1 << inst[2:0]) & {8{~inst[7] & ~inst[6] & ~inst[5] & inst[4] & inst[3]}};

    control testGate(
        .inst(inst),
        .cycle(cycle),
        .carry(carry),
        
        .M(M),
        .S(S),
        .J(J),
        .LJ(LJ),
        .CLI(CLI),
        .LJR(LJR),
        .MW(MW),
        .MC(MC),
        .RD(RD),
        .WR(WR),
        .Y(Y),
        .RS(RS),
        .WA(WA),
        .ISP(ISP),
        .WC(WC),
        .ALU(ALU),
        .SIG(SIG)
    );

    combitest #("control", 10, 16, 1000) tester(
        .comp_in({inst, cycle, carry}),
        .verify({realM, realS, realJ, realLJ, realCLI, realLJR, realMC, realRD, realWR, realY, realRS, realALU}),
        .comp_out({M, S, J, LJ, CLI, LJR, MC, RD, WR, Y, RS, ALU})
    );

    initial begin
        $dumpfile(`WAVEPATH);
        $dumpvars;
    end
endmodule