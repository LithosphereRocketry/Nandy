module tb_rom();
    wire [14:0] addr;
    wire [7:0] ans;
    wire [7:0] realans;
    assign realans = (addr & 8'hFF) ^ (addr >> 8);

    rom #("tests/FUZZ.txt") testGate(
        .address(addr),
        .data(ans)
    );

    combitest #("rom", 15, 8, 151) tester(
        .comp_in(addr),
        .verify(realans),
        .comp_out(ans)
    );
endmodule