module assert #(parameter message = "unspecified")
               (input value);
    task test;
        if (~value) begin
            $display("Assertion failed: %s", message);
            $finish(1);
        end
    endtask
endmodule