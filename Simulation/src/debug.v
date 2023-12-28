`timescale 1ns/1ps

module assert #(parameter message = "unspecified")
               (input value);
    task test;
        if (~value) begin
            $display("ERROR: Assertion failed: %s", message);
            $finish(1);
        end
    endtask
endmodule

module combitest #(parameter name = "unnamed", parameter inbits = 1,
                   parameter outbits = 1, parameter delay = 10,
                   parameter cooldown = 1000)
                  (output reg [(inbits-1):0] comp_in,
                   input wire [(outbits-1):0] verify,
                   input wire [(outbits-1):0] comp_out);
    
    reg [(inbits-1):0] counter = 0;

    initial begin
        $dumpfile(`WAVEPATH);
        $dumpvars;
        repeat (1 << inbits) begin
            comp_in = {inbits{1'bx}};
            #(cooldown);
            comp_in = counter;
            counter = counter + 1;
            #(delay);
            if (comp_out !== verify) begin
                $display("ERROR: Test %s failed: input %b -> expected %b, got %b",
                    name, comp_in, verify, comp_out);
                $finish(1);
            end
        end
        $finish(0);
    end
endmodule

module randomizer #(parameter width = 1) (output reg [(width-1):0] value);
    task roll;
        value = {((width+31)/32){$random}} & {width{1'b1}};
    endtask
endmodule

module randtest #(parameter name = "unnamed", parameter inbits = 1,
                 parameter outbits = 1, parameter delay = 10,
                 parameter cooldown = 1000)
                (output reg [(inbits-1):0] comp_in,
                 input wire [(outbits-1):0] verify,
                 input wire [(outbits-1):0] comp_out);
    
    wire [(inbits-1):0] value;
    randomizer #(inbits) r (.value(value));

    initial begin
        $dumpfile(`WAVEPATH);
        $dumpvars;
        repeat (16384) begin
            comp_in = {inbits{1'bx}};
            #(cooldown);
            r.roll();
            comp_in = value;
            #(delay);
            if (comp_out !== verify) begin
                $display("ERROR: Test %s failed: input %b -> expected %b, got %b",
                    name, comp_in, verify, comp_out);
                $finish(1);
            end
        end
        $finish(0);
    end
endmodule