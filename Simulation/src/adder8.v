`timescale 1ns/1ps

module adder8(
        input [7:0] a,
        input [7:0] b,
        input cin,
        output [7:0] q,
        output cout
    );
    
    wire [6:0] carrythru;

    fulladder adders [7:0] (
        .a(a),
        .b(b),
        .cin({carrythru, cin}),
        .q(q),
        .cout({cout, carrythru})
    );
endmodule