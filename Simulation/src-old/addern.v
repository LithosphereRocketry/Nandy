`timescale 1ns/1ps

module addern #(parameter WIDTH = 1) (
        input [(WIDTH-1):0] a,
        input [(WIDTH-1):0] b,
        input cin,
        output [(WIDTH-1):0] q,
        output cout
    );
    
    wire [(WIDTH-2):0] carrythru;

    fulladder adders [(WIDTH-1):0] (
        .a(a),
        .b(b),
        .cin({carrythru, cin}),
        .q(q),
        .cout({cout, carrythru})
    );
endmodule