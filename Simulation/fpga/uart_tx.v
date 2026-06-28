module uart_tx #(
        parameter DATA_BITS = 8,
        parameter BAUD_RATE = 9600,
        parameter CLK_RATE = 48000000,

        // inexact, but fine for now - can deal with better dividing later
        localparam DIVISOR = CLK_RATE / BAUD_RATE,
        
        // State machine enum
        localparam ST_IDLE = DATA_BITS,
        localparam ST_START = DATA_BITS+1,
        localparam ST_STOP = DATA_BITS+2,
        localparam ST_COUNT = DATA_BITS+3
    ) (
        input clk,

        input [DATA_BITS-1:0] data,
        input data_valid,
        output data_ready,

        output txd
    );

    reg [$clog2(ST_COUNT)-1:0] state = ST_IDLE;
    reg [$clog2(DIVISOR)-1:0] divider;
    reg [$clog2(DATA_BITS)-1:0] bitcount;
    reg [DATA_BITS-1:0] shift_reg;

    assign data_ready = (state == ST_IDLE);

    assign txd = state == ST_IDLE ? 1'b1
               : state == ST_START ? 1'b0
               : state == ST_STOP ? 1'b1
               : shift_reg[0];

    always @(posedge clk) begin
        divider <= divider - 1;
        if(state == ST_IDLE) begin
            if(data_valid) begin
                state <= ST_START;
                divider <= DIVISOR;
                shift_reg <= data;
            end
        end else if(state == ST_START && divider == 0) begin
            state <= DATA_BITS-1;
            divider <= DIVISOR;
        end else if(state == ST_STOP && divider == 0) begin
            state <= ST_IDLE;
        end else if(state == 0 && divider == 0) begin
            state <= ST_STOP;
            divider <= DIVISOR;            
        end else if(divider == 0) begin
            state <= state-1;
            divider <= DIVISOR;
            shift_reg <= shift_reg >> 1;
        end
    end

endmodule