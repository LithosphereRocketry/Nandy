`timescale 1ns/1ps

module signext #(parameter widthIn = 1, parameter widthOut = 1) (
        input [widthIn-1:0] in,
        output [widthOut-1:0] out
    );

    assign out = {{widthOut-widthIn{in[widthIn-1]}}, widthIn};
endmodule