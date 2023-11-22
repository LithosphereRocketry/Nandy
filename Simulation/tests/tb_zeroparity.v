module tb_zeroparity();
    wire [7:0] data;
    wire mode;
    wire ans, realans;
    assign realans = mode ? (data[0] ^ data[1] ^ data[2] ^ data[3]
                           ^ data[4] ^ data[5] ^ data[6] ^ data[7])
                          : (data != 0);

    zeroparity testGate(
        .data(data),
        .mode(mode),
        .q(ans)
    );

    combitest #("zeroparity", 9, 1, 94) tester(
        .comp_in({mode, data}),
        .verify(realans),
        .comp_out(ans)
    );
endmodule