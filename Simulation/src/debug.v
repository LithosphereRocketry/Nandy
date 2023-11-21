`timescale 1ns/1ps

module assert #(parameter message = "unspecified")
               (input value);
    task test;
        if (~value) begin
            $display("Assertion failed: %s", message);
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
        repeat (1 << inbits) begin
            comp_in = {inbits{1'bx}};
            #(cooldown);
            comp_in = counter;
            counter = counter + 1;
            #(delay);
            if (comp_out !== verify) begin
                $display("Test %s failed: input %b -> expected %b, got %b",
                    name, comp_in, verify, comp_out);
                $finish(1);
            end
        end
        $finish(0);
    end
endmodule