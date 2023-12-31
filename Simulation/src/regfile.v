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
        input [7:0] ioin,
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

    // Make an inverted copy of register-select to be shared by the decoders
    wire [1:0] nRS;
    invert invrs [1:0] (.a(RS), .q(nRS));
    // And one for interrupt status & interrupt enable
    wire nistatus;
    invert invis(.a(istatus), .q(nistatus));
    wire nienabled;
    invert invie(.a(ienabled), .q(nienabled));

    // Accumulator write decision logic - this is just an or since the logic is
    // pretty simple while being annoying to optimize
    wire wracc;
    orgate gwracc(
        .a(RD),
        .b(WA),
        .q(wracc)
    );

    // Select accumulator input; normally gets the ALU, except on rd-mode
    // instructions where it gets passed a different register's contents
    wire [7:0] accin;
    wire [7:0] regpass;
    mux #(8) macc(
        .a(aluout),
        .b(regpass),
        .s(RD),
        .q(accin)
    );
    // The accumulator itself, after all that pain
    register #(8) accumulator(
        .d(accin),
        .clk(clk),
        .en(wracc),
        .nclr(1'b1),
        .q(acc)
    );
    
    // Stack pointer write decision logic - gmvsp here acts as 1/4 of a decoder
    // while also providing the AND input for write-mode instructions
    wire nmvsp;
    nand10 gmvsp(
        .a(nRS[1]),
        .b(nRS[0]),
        .c(WR),
        .q(nmvsp)
    );
    // Using the and-or trick here to save inverts
    // i.e. (A & B) | C === (A NAND B) NAND ~C
    wire wrsp;
    nand00 gwrsp(
        .a(nmvsp),
        .b(nISP),
        .q(wrsp)
    );
    // SP normally comes from ALU (as in isp), or ACC on wr-mode
    wire [7:0] spin;
    mux #(8) msp(
        .a(accin),
        .b(acc),
        .s(WR),
        .q(spin)
    );
    // And the register:
    register #(8) stackpointer(
        .d(spin),
        .clk(clk),
        .en(wrsp),
        .nclr(1'b1),
        .q(sp)
    );

    // Whether to update the interrupt-return address; shared across IRX/IRY
    wire naltfromint;
    nand00 galtfromint(
        .a(nistatus),
        .b(ienabled),
        .q(naltfromint)
    );

    // DX write decision is a bit more complicated; we have to demux between
    // the main and alternate versions of DX so that it works properly in
    // interrupts
    // First figure out if either DX is written:
    wire nmvdx;
    nand10 gmvdx(
        .a(RS[1]),
        .b(nRS[0]),
        .c(WR),
        .q(nmvdx)
    );
    wire wranyx;
    nand00 ganyx(
        .a(nmvdx),
        .b(nLJR),
        .q(wranyx)
    );
    // Then enable only the register that's actually correct:
    wire wrmainx;
    andgate gmainx(
        .a(wranyx),
        .b(nistatus),
        .q(wrmainx)
    );
    // Alt register write is a little more complicated, since they can also be
    // written in the background by the interrupt system
    wire naltxfromreg;
    nand00 galtxfromreg(
        .a(wranyx),
        .b(nistatus),
        .q(naltxfromreg)
    );
    // and/or trick here as usual
    wire wraltx;
    nand00 galtx(
        .a(naltxfromreg),
        .b(naltfromint),
        .q(wraltx)
    );

    // Input selection - "normal" is jsr, "alternate" is wr-mode
    wire [7:0] dxin;
    mux #(8) minx(
        .a(RA[7:0]),
        .b(acc),
        .s(WR),
        .q(dxin)
    );
    // Of course, IRX has an extra wrinkle here as well; when an interrupt is
    // not active it has to pull data from the interrupt return address instead
    wire [7:0] altxin;
    mux #(8) maltx(
        .a(dxin),
        .b(intRA[7:0]),
        .s(nistatus),
        .q(altxin)
    );
    // Finally the registers:
    wire [7:0] normaldx, altdx;
    register #(8) datax(
        .d(dxin),
        .clk(clk),
        .en(wrmainx),
        .nclr(1'b1), 
        .q(normaldx)
    );
    register #(8) intretx(
        .d(altxin),
        .clk(clk),
        .en(wraltx),
        .nclr(1'b1),
        .q(altdx)
    );
    // And of course since nothing is easy, we also have to mux the outputs:
    mux #(8) mdx(
        .a(normaldx),
        .b(altdx),
        .s(istatus),
        .q(dx)
    );

    // And the whole mess has to be repeated for DY
    wire nmvdy;
    nand10 gmvdy(
        .a(RS[1]),
        .b(RS[0]),
        .c(WR),
        .q(nmvdy)
    );
    wire wranyy;
    nand00 ganyy(
        .a(nmvdy),
        .b(nLJR),
        .q(wranyy)
    );
    wire wrmainy;
    andgate gmainy(
        .a(wranyy),
        .b(nistatus),
        .q(wrmainy)
    );
    wire naltyfromreg;
    nand00 galtyfromreg(
        .a(wranyy),
        .b(nistatus),
        .q(naltyfromreg)
    );
    wire wralty;
    nand00 galty(
        .a(naltyfromreg),
        .b(naltfromint),
        .q(wralty)
    );
    wire [7:0] dyin;
    mux #(8) miny(
        .a(RA[15:8]),
        .b(acc),
        .s(WR),
        .q(dyin)
    );
    wire [7:0] altyin;
    mux #(8) malty(
        .a(dyin),
        .b(intRA[15:8]),
        .s(nistatus),
        .q(altyin)
    );
    wire [7:0] normaldy, altdy;
    register #(8) datay(
        .d(dyin),
        .clk(clk),
        .en(wrmainy),
        .nclr(1'b1), 
        .q(normaldy)
    );
    register #(8) intrety(
        .d(altyin),
        .clk(clk),
        .en(wralty),
        .nclr(1'b1),
        .q(altdy)
    );
    mux #(8) mdy(
        .a(normaldy),
        .b(altdy),
        .s(istatus),
        .q(dy)
    );

    // The IO register is refreshingly less painful, just a 3 input and to
    // determine writes and we only pul from accumulator
    // Note that the write signal is connected externally to OnWrite
    and3 gwrio(
        .a(nRS[1]),
        .b(RS[0]),
        .c(WR),
        .q(OnWrite)
    );
    register #(8) ioreg(
        .d(acc),
        .clk(clk),
        .en(OnWrite),
        .nclr(1'b1),
        .q(ioout)
    );

    // Now the other fun part: the endless tangle of output muxes
    // 4-way mux to generate the register passback:
    wire [7:0] regpass_spio;
    mux #(8) mregpass_spio(
        .a(sp),
        .b(ioin),
        .s(RS[0]),
        .q(regpass_spio)
    );
    // This mux serves both ALU output 2 and the passback mux, so we mux its
    // control signal between the two - with some clever optimization a few
    // gates can probably be saved here
    wire seldxdy;
    mux #(1) mseldxdy(
        .a(Y),
        .b(RS[0]),
        .s(RD),
        .q(seldxdy)
    );
    wire [7:0] dxdy;
    mux #(8) mdxdy(
        .a(dx),
        .b(dy),
        .s(seldxdy),
        .q(dxdy)
    );
    mux #(8) mregpass(
        .a(regpass_spio),
        .b(dxdy),
        .s(RS[1]),
        .q(regpass)
    );
    // If we're on the second cycle, ALUB is always going to want the memory
    // output:
    wire [7:0] xymem;
    mux #(8) mxymem(
        .a(dxdy),
        .b(mem),
        .s(cycle),
        .q(xymem)
    );
    // isp is weird because it has both a unique ALUA and ALUB, so it gets its
    // own pair of muxes. Also, 
    mux #(8) mouta(
        .a(sp),
        .b(acc),
        .s(nISP),
        .q(A)
    );
    wire [7:0] immext;
    signext #(4, 8) gimmext(
        .in(imm),
        .out(immext)
    );
    mux #(8) moutb(
        .a(immext),
        .b(xymem),
        .s(nISP),
        .q(B)
    );

    // OnRead signal
    // These are subject to change - they may want to be latched
    // Note that OnWrite was generated incidentally by the IO register write
    // logic
    and3 gonread(
        .a(nRS[1]),
        .b(RS[0]),
        .c(RD),
        .q(OnRead)
    );
endmodule