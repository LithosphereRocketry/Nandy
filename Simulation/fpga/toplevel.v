module toplevel(
        input clk48,
        output rst_n,

        output rgb_led0_r,
        output rgb_led0_g,
        output rgb_led0_b,

        output gpio_1, // uart tx

        input usr_btn
    );

    reg [23:0] counter = 0;
    always @(posedge clk48) counter <= counter+1;
    assign rgb_led0_r = counter[23];
    assign rgb_led0_g = 1;
    assign rgb_led0_b = 1;

    uart_tx uart(
        .clk(clk48),

        .data(65),
        .data_valid(counter[23:16] == 0),
        .data_ready(),

        .txd(gpio_1)
    );

    orangecrab_reset #(48000000) rst(
        .clk(clk48),
        .do_reset(~usr_btn),
        .nreset_out(rst_n)
    );

endmodule