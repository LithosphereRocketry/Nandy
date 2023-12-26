`timescale 1ns/1ps

module tb_ram();
    reg [14:0] addr;
    reg wr;
    reg [7:0] din;
    wire [7:0] data;
    assign data = din;
    ram r(
        .address(addr),
        .write(wr),
        .data(data)
    );

    assert #("Memory at addr 0 is wrong") a1(.value(data === 8'h12));
    assert #("Memory at addr 100 is wrong") a2(.value(data === 8'h34));
    initial begin
        $dumpfile(`WAVEPATH);
        $dumpvars;
        
        wr = 1;
        addr = 0;
        din = 8'hzz;
        #100;
        din = 8'h12;
        wr = 0;
        #100;
        wr = 1;
        addr = 100;
        #100;
        din = 8'h34;
        wr = 0;
        #100;
        wr = 1;
        addr = 0;
        din = 8'hzz;
        #100;
        a1.test();
        #100;
        addr = 100;
        #100;
        a2.test();
        #100;

        $finish();
    end
    
endmodule