/*
Address calculation datapath for NANDy CPU. Denoted in purple in block diagram.
*/

module addr_calc(
        input clk;

        input use_add, do_interrupt, in_interrupt, wr_pc;
        input [1:0] base_sel;

        input [15:0] immediate;
        input [15:0] pq;
        input [7:0] sp;
        
        output [15:0] addr;
    );

    wire [15:0] int_vector = 16'h7F00;

    reg [15:0] pc, ia;

    wire [15:0] addr_base;
    mux4 muxbase [1:0](
        .a(pc),
        .b(ia),
        .c(pq),
        .d({7'h7F, in_interrupt, sp}),
        .sel(base_sel),
        .q(addr_base)
    )


    wire [15:0] addr_offset;

    wire [15:0] addr_sum = addr_base + addr_offset;

    assign addr = use_add ? addr_sum : pc;

    wire [15:0] pc_in = do_interrupt ? ;

    always @(posedge clk) begin
        if(wr_ia) ia <= pc;
        if(wr_pc) pc <= pc_in;
    end

endmodule