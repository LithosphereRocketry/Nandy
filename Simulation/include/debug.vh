`ifndef DEBUG_VH
`define DEBUG_VH

task assert (input condition);
    begin:assert_blk
        parameter message = "no information given";
        if (!condition) begin
            $display ("Assertion failed: %s", message);
            $finish(1);
        end
    end
endtask

`endif