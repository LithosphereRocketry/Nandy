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
##### `bell`
Plays an audible alert tone.
##### `eint`, `dint`
Enables or disables interrupts, respectively.
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
#### Loads
##### `lda <offset>`
Loads the value at the combined address stored in y and x plus the specified
unsigned 4-bit offset into the accumulator.
##### `lds <offset>`
Loads the value at the address formed by 0xFF00 + SP + offset into the
accumulator.
#### Stores
##### `stra <offset>`
Stores the value of the accumulator at the combined address stored in y and x
plus the specified unsigned 4-bit offset.
##### `strs <offset>`
Stores the value of the accumulator at the address formed by 0xFF00 + SP +
offset.

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