# NANDy Instruction Reference

## Introduction
This document is meant as a supplement to the NANDy Programmer's guide. It
provides a concise listing of all hardware-supported instructions for the NANDy
CPU architecture.

## Instruction Reference

### General-purpose

#### Control
##### `nop`
Does nothing.
##### `brk`
Stops program execution. In emulation, exits to the debugger interface.
##### `bell`
Plays an audible alert tone.
##### `eint`, `dint`
Enables or disables interrupts, respectively.
##### `iset`, `iclr`
Sets or clears interrupt status, respectively. Does not trigger an interrupt;
behavior is undefined while external interrupts are enabled.
##### `j <label>`
Jumps to the specified label.
##### `jcz <label>`
Jumps to the specified label if the carry bit is 0.
##### `ja`
Jumps to the address formed by the combination of DY and DX.
##### `jar`
Identical to `ja`, but replaces the contents of DY and DX with the address of
the following instruction.
##### `jri`
Identical to `ja`, but also clears interrupt status if it is present.

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
##### `add <dx/dy>`
Adds the contents of the specified register to the accumulator.
##### `addc <dx/dy>`
Identical to `add` but treats the carry bit as a carry-in bit from a less
significant byte.
##### `sub <dx/dy>`
Subtracts the contents of the specified register from the accumulator.
##### `subc <dx/dy>`
Identical to `sub` but treats the carry bit as a carry-in bit from a less
significant byte.
##### `_add`, `_sub`, `_addc`, `_subc`
Identical to their respective non-underscore variants, but do not modify the
carry bit.
##### `addi`, `addci`, `subi`, `subci`, `_addi`, `_addci`, `_subi`, `_subci`
Identical to their respective non-`i` variants, but use the provided immediate
value in place of the DX or DY register.

#### Comparison
##### `zero`, `nzero`
Sets the carry bit to 1 if the accumulator does or does not hold the value 0,
respectively, and sets it to zero otherwise.
##### `par`, `npar`
Sets the carry bit to 1 if the accumulator contains an odd or even number of
high bits, respecitvely.

#### Bitwise logic
##### `and <dx/dy>`
Bitwise-ands the contents of the accumulator with the specified register and
stores the results in the accumulator.
##### `nand <dx/dy>`
Bitwise-nands the contents of the accumulator with the specified register and
stores the results in the accumulator.
##### `or <dx/dy>`
Bitwise-ors the contents of the accumulator with the specified register and
stores the results in the accumulator.
##### `andi`, `nandi`, `ori`
Identical to their respective non-`i` variants, but use the provided immediate
value in place of the DX or DY register.

#### Shifts
For all of the following, the carry bit is set to the value shifted off the end
of the accumulator.
##### `sl`, `sr`
Shifts the accumulator left or right, respectively, by one bit, inserting a 0 at
the vacated place.
##### `slc`, `src`
Shifts the accumulator left or right, respectively, by one bit, inserting the
previous value of the carry bit at the vacated place.
##### `sla`, `sra`
Shifts the accumulator left or right, respectively, by one bit, copying the
value of the vacated place from the bit adjacent to it.
##### `slr`, `srr`
Shifts the accumulator left or right, respectively, by one bit, copying the
value of the vacated place from the bit shifted off the end.
##### `_sl`, `_sr`, `_slc`, `_src`, `_sla`, `_sra`, `_slr`, `_srr`
Identical to their respective non-underscore variants, but do not modify the
carry bit.

### Memory
#### Stack pointer
##### `isp`
Increments the stack pointer by the provided signed 4-bit value, and sets the
carry bit to the carry-out value of the addition or subtraction.
##### `_isp`
Identical to `isp` but does not set the carry bit.
#### Loads
##### `lda <offset>`
Loads the value at the combined address stored in DY and DX plus the specified
unsigned 4-bit offset into the accumulator.
##### `lds <offset>`
Loads the value at the address formed by 0xFF00 + SP + offset into the
accumulator.
#### Stores
##### `stra <offset>`
Stores the value of the accumulator at the combined address stored in DY and DX
plus the specified unsigned 4-bit offset.
##### `strs <offset>`
Stores the value of the accumulator at the address formed by 0xFF00 + SP +
offset.