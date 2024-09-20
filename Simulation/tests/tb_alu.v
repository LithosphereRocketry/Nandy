module tb_alu();
    wire [7:0] a;
    wire [7:0] b;
    wire [3:0] op;
    wire cin, xy;
    wire [7:0] ans;
    wire carry;
    reg [7:0] realans;
    reg realcarry;
    
    always @(*) begin
        case (op)
            4'h0: begin
                realans = b;
                realcarry = xy;
            end
            4'h1: begin
                realans = a | b;
                realcarry = cin ^ xy;
            end
            4'h2: begin
                realans = a & b;
                realcarry = (a != 0) ^ xy;
            end
            4'h3: begin
                realans = a ^ b;
                realcarry = (a[0] ^ a[1] ^ a[2] ^ a[3]
                           ^ a[4] ^ a[5] ^ a[6] ^ a[7]) ^ xy;
            end
            4'h4: begin
                {realcarry, realans} = a + b;
            end
            4'h5: begin
                {realcarry, realans} = a + b + cin;
            end
            4'h6: begin
                {realcarry, realans} = a + ((~b) & 8'hff) + 1'b1;
            end
            4'h7: begin
                {realcarry, realans} = a + ((~b) & 8'hff) + cin;
            end
            4'h8: begin
                realans = a << 1;
                realcarry = a[7];
            end
            4'h9: begin
                realans = a << 1 | cin;
                realcarry = a[7];
            end
            4'ha: begin
                realans = a << 1 | a[0];
                realcarry = a[7];
            end
            4'hb: begin
                realans = a << 1 | a[7];
                realcarry = a[7];
            end
            4'hc: begin
                realans = a >> 1;
                realcarry = a[0];
            end
            4'hd: begin
                realans = (a >> 1) | (cin << 7);
                realcarry = a[0];
            end
            4'he: begin
                realans = (a >> 1) | (a[0] << 7);
                realcarry = a[0];
            end
            4'hf: begin
                realans = (a >> 1) | (a[7] << 7);
                realcarry = a[0];
            end
        endcase
    end

    initial begin
        $dumpfile(`WAVEPATH);
        $dumpvars;
    end

    alu testGate(
        .a(a),
        .b(b),
        .op(op),
        .xy(xy),
        .cin(cin),
        .q(ans),
        .cout(carry)
    );

    randtest #("alu", 22, 9, 1000) tester(
        .comp_in({xy, op, a, b, cin}),
        .verify({realcarry, realans}),
        .comp_out({carry, ans})
    );
endmodule