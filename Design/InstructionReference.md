# NANDy Instruction Reference

## Introduction
This document is meant as a supplement to the NANDy Programmer's guide. It
provides a concise listing of all hardware-supported instructions for the NANy
CPU architecture.

## Instruction Reference

### General-purpose

#### Control
##### `nop` [^1]
Does nothing.
##### `brk`
Stops program execution. In emulation, exits to the debugger interface.
##### `j <label>`
Jumps to the specified label.
##### `jr <label>`
Jumps to the specified label, storing the current PC in `p`. [^2]
##### `jc <label>`
Jumps to the specified label if the carry bit is set.
##### `jcr <label>`
Jumps to the specified label if the carry bit is set, storing the current PC in
`p`. [^2]
##### `jp`
Jumps to one byte past the address stored in `p`. [^3]
##### `jpr`
Jumps to one byte past the address stored in `p`, storing the current PC in `p`. [^3]
##### `jxi`
Jumps to the address stored in the hidden `IA` register and clears the interrupt
flag.

#### Status
##### `eint`, `dint`
Enables or disables interrupts, respectively.
##### `ipoll`
Polls the processor status and stores the result in the accumulator. Status is
encoded as follows:

* Bit 7: Interrupt active
* Bit 6: Interrupts enabled
* Bits 5-0: IRQ lines 5-0

#### Registers
##### `rd <register>`
Moves the contents of the specified register into the accumulator.
##### `wr <register>`
Moves the contents of the accumulator into the specified register.
##### `sw <register>`
Exchanges the contents of the accumulator with the specified register.
##### `rdi <value>`
Loads the specified 8-bit value into the accumulator.
##### `cset`, `cclr`
Sets or clears the carry bit, respectively.
##### `ctog`
Toggles the value of the carry bit.

### Arithmetic and Logic

#### Addition & subtraction
For all of the following, the carry bit is set to the carry-out value of the
operation.
##### `add <x/y>`
Adds the contents of the specified register to the accumulator.
##### `adc <x/y>`
Adds the contents of the specified register plus the carry bit to the
accumulator, and sets the carry bit to the carry-out value of the result.
##### `sub <x/y>`
Subtracts the contents of the specified register from the accumulator.
##### `sbc <x/y>`
Subtracts the contents of the specified register minus the inverse of the carry
bit from the accumulator, and sets the carry bit to the carry-out value of the
result.

#### Comparison
##### `zero`, `nzero`
Sets the carry bit to 1 if the accumulator does or does not hold the value 0,
respectively, and sets it to zero otherwise.
##### `sgn`, `nsgn`
Sets the carry bit to the sign bit or inverse of the sign bit of the
accumulator, respectively.

#### Bitwise logic
##### `and <x/y>`
Bitwise-ands the contents of the accumulator with the specified register and
stores the results in the accumulator.
##### `or <x/y>`
Bitwise-ors the contents of the accumulator with the specified register and
stores the results in the accumulator.
##### `xor <x/y>`
Bitwise-xors the contents of the accumulator with the specified register and
stores the results in the accumulator.
##### `andi`, `ori`, `xori`
Identical to their respective non-`i` variants, but use the provided immediate
value in place of the x or y register.

#### Shifts
For all of the following, the carry bit is set to the value shifted off the end
of the accumulator.
##### `sl`, `sr`
Shifts the accumulator left or right, respectively, by one bit, inserting a 0 at
the vacated place.
##### `slc`, `src`
Shifts the accumulator left or right, respectively, by one bit, inserting the
previous value of the carry bit at the vacated place, and setting the carry bit
to the shifted-out value of the result.

### Memory
#### `ld <mode> <offs>`
Loads the accumulator from memory at the address defined by the addressing mode
plus the specified unsigned 16-bit offset. Addressing modes are as follows:

* `sp`: Base address is the stack pointer, with an upper byte of 0xFE if not in
  an interrupt handler or 0xFF if in an interrupt handler. Unlike other modes,
  offsets wrap on 256-byte boundary.
* `q`: Base address is the contents of the `q` pointer register.
* `p`: Base address is the contents of the `p` pointer register.
* `+p`: Result address is the same as the `p` addressing mode, except that
  `p + offs` is stored to `p` after the access.

#### `st <mode> <offs>`

Stores the contents of the accumulator to memory at the address defined by the
addressing mode plus the specified unsigned 16-bit offset. Addressing modes are
the same as in the `ld` instruction.

[^1]: At several places in NANDy internals `nop` is referred to as a comparison
    instruction. This is because, internally, it's implemented as "set carry
    equal to carry" - the non-inverted form of `ctog`. (The instructions `cset`,
    `cclr`, and `ctog` are also internally treated as comparisons.)

[^2]: When a two-byte instruction stores the current value of PC, it stores the
    address of the second byte of the instruction. This allows `jp` to the
    resultant address to jump to the following instruction.

[^3]: Register-absolute jumps target the byte after the given address because of
    NANDy's two-cycle-per-instruction limit. The full task of making a relative
    function call requires three 16-bit adds:
    * Compute PC+1 to load the second byte of the instruction.
    * Compute PC+offset to determine the target of the jump.
    * Compute PC+2 to determine the return address of the jump.
    By making the stored return address equal to PC+1 instead of PC+2, we can
    offload the computation of PC+2 to the return step, allowing `jr` to fit in
    two cycles.