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
    wire realS;
    wire realJ;
    wire realLJ;
    wire realCLI;
    wire realLJR;
    wire realMW;
    
    wire realMC;
    assign realMC = inst[7] & ~cycle;

    wire realRD;
    wire realWR;
    wire realY;
    
    wire [1:0] realRS;
    assign realRS = inst[1:0];

    wire realWA;
    wire realISP;
    wire realWC;

    wire [3:0] realALU;
    assign realALU = inst[3:0];

    wire [7:0] realSIG;

    control testGate(
        .inst(inst),
        .cycle(cycle),
        .carry(carry),
        
        // wire M;
        // wire S;
        // wire J;
        // wire LJ;
        // wire CLI;
        // wire LJR;
        // wire MW;
        // wire MC;
        // wire RD;
        // wire WR;
        // wire Y;
        .RS(RS),
        // wire WA;
        // wire ISP;
        // wire WC;
        .ALU(ALU)
        // wire [7:0] SIG;
    );

    combitest #("control", 10, 6, 1000) tester(
        .comp_in({inst, cycle, carry}),
        .verify({realRS, realALU}),
        .comp_out({RS, ALU})
    );
endmodule