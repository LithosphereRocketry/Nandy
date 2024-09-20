# Proposed Changes for NANDy Architecture v5.0

* Remove hysteresis from interrupt line.
    * Size impact: remove 1-2 chips
    * Performance impact: Minimal
    * Usability impact: Adds additional state requirement for I/O devices;
      reduces usefulness of "raw interrupts" (e.g. connecting a switch directly
      to the interrupt line)
    * Justification: If multiple I/O devices are multiplexed on the interrupt
      line, it may be difficult to keep track of which interrupts have been
      handled and which have not. By making I/O devices manually clear
      interrupts, the CPU can continually respond to interrupts, one after
      another, until all have been cleared.
* Add instructions `csset` and `csclr`.
    * Size impact: add 1-2 chips
    * Performance impact: minimal
    * Usability impact: Allows easier multiplexing of I/O devices
    * Justification: Previously, an external I/O multiplexer would have to
      either restrict the I/O space available to external devices or require a
      control sequence to be sent before any group of I/O in order to
      distinguish commands sent to the controller from commands sent to the
      peripheral devices. The presence of a chip-select flag allows commands to
      be routed to a controller without the need for this additional complexity.
* Remove inverted bitwise instructions.
    * Size impact: remove 8 chips
    * Performance impact: 1-2 extra cycles per usage
    * Usability impact: Replaceable with 2-instruction sequence or macro
    * Justification: These instruction types can be directly replaced by
      a 2-instruction sequence: e.g. `nand` can be replaced by `and; xori 0xFF`.
* Add instruction `inc`/`_inc`.
    * Size impact: add approx. 8 chips
    * Performance impact: 1 fewer cycle for loop counters and other small-value
      additions
    * Usability impact: Some added confusion betwteen `inc` and `addi`, but
      otherwise minimal
    * Justification: Nearly all uses of `addi` are to increment a loop counter
      or pointer by a small number. This instruction would take the opcode space
      freed by the removal of the bitwise/comparison operations, and would allow
      increasing or decreasing a value by up to 3 in a single cycle. This can
      give a performance gain of up to 10% in tight loops.
* Add instruction `acf`/`_acf`.
    * Size impact: add approx. 2 chips
    * Performance impact: 1 fewer cycle per additional byte in wide additions
    * Usability impact: Some added confusion betwteen `acf` and `addci 0`, but
      otherwise minimal
    * Justification: A relatively common operation in larger programs is pointer
      arithmetic, which requires a sequence like `addi 2; sw dx; addci 0`. While
      `inc` allows the `addi 2` component to occur in 1 cycle, `acf` does the
      same for the `addci 0` component.


## TODO Status
* Arithmetic changes
  * Verilog: started
* Interrupt changes
  * Toolchain: done, mostly
  * Logisim: not started
  * Verilog: not started
  * Documentation: done
* Chip select
  * Toolchain: in progress
  * Logisim: not started
  * Verilog: not started
  * Documentation: done