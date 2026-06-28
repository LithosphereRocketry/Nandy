module toplevel(
        input clk48,
        output rst_n,

        output rgb_led0_r,
        output rgb_led0_g,
        output rgb_led0_b,

        output gpio_1, // uart tx

        input usr_btn
    );

    reg [5:0] cpu_strobe_counter = 0;
    always @(posedge clk48) cpu_strobe_counter <= cpu_strobe_counter+1;
    wire cpu_strobe = (cpu_strobe_counter == 0);

    wire [7:0] io_addr, io_out;
    wire io_wr;
    wire uart_ready;
    // only 2K of RAM to save on LUTRAM since our memory is async and therefore
    // quite expensive on FPGA
    core #(`ROMPATH, 11) cpu(
        .clk(clk48),
        // for now, stop the world when the uart is busy
        .clken(cpu_strobe & uart_ready),
        .rst(~usr_btn),
        
        .io_in(8'h00),
        .ints_in(6'h00),
        .io_addr(io_addr),
        .io_out(io_out),
        .wr_io(io_wr)
    );

    uart_tx uart(
        .clk(clk48),

        .data(io_out),
        .data_valid(cpu_strobe & io_wr & io_addr == 8'h00),
        .data_ready(uart_ready),

        .txd(gpio_1)
    );

    orangecrab_reset #(48000000) rst(
        .clk(clk48),
        .do_reset(~usr_btn),
        .nreset_out(rst_n)
    );

endmodule