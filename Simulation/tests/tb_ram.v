`timescale 1ns/1ps

module tb_ram();
    wire [7:0] data;
    reg [7:0] dataReg;
    reg [7:0] addr;
    reg write;

    assign data = !write ? dataReg : 8'bz;
    
    ram test(
        .data(data),
        .address(addr),
        .write(write)
    );

    initial begin
        dataReg = 8'bx;
        addr = 8'h0;
        write = 1'b1;
        #80;
        dataReg = 8'hAA;
        #20;
        write = 1'b0;
        #100;
        write = 1'b1;
        #80;
        addr = 8'h4;
        #20;
        dataReg = 'hFE;
        write = 1'b0;
        #100;
        write = 1'b1;
        addr = 8'h0;
        #100;
        addr = 8'h4;
        #100;
    end

endmodule