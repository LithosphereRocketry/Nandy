`timescale 1ns/1ps

module tb_ram();
    wire [7:0] data;
    reg [7:0] dataReg;
    reg [7:0] addr;
    reg write;

    assign data = !write ? dataReg : 'z;
    
    ram test(
        .data(data),
        .address(addr),
        .write(write)
    );

    initial begin
        dataReg = 'x;
        addr = 0;
        write = 1;
        #80;
        dataReg = 'hAA;
        #20;
        write = 0;
        #100;
        write = 1;
        #80;
        addr = 4;
        #20;
        dataReg = 'hFE;
        write = 0;
        #100;
        write = 1;
        addr = 0;
        #100;
        addr = 4;
        #100;
    end

endmodule