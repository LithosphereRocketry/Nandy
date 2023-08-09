# NANDy Programmer's Manual

NOTE: implementation of these features not yet complete

## About This Guide
This guide is intended to detail all of the explicit rules, as well as some best practices and implementation hints, for writing programs in NANDy assembly. Because the NANDy is a very simple processor with limited resources, it may often be necessary to deviate from these guidelines for best performance, so this document can and should be ignored whenever it is necessary; however, following these conventions will make code significantly easier to understand.  

Throughout this guide, `code markdown` is used for code snippets, instruction specifications, and other pieces of useful text. In general, code markdown with `<angle brackets>` is used to denote a required parameter, while `[square brackets]` are used to denote optional syntax.

## The NANDy Assembler

### Basic Syntax
NANDy provides a referencce assembler design which supports all *(not yet) core instructions as well as a number of helpful shortcuts and macros, which are detailed in their relevant sections throughout this guide. The basic structure of a line of assembly is:
```
[<label>:] <command> [<operands>] [#<comment>]
```
It is also allowable to have a line with only a label, only a comment, or no content at all. Arguments are whitespace-delineated, and it is not necessary to have any whitespace after a label or before a comment; all whitespace other than newlines is considered equivalent.

By convention, commands starting with `@` represent preprocessing macros that expand to either nothing or complex external constructs; other assemblers are very strongly discouraged (but not prohibited) from using the `@` character in any other context.

### Literals and Symbols
Several instructions take a literal value as an argument; this can be a decimal number *(more radices coming soon) or a combination of operations on another literal. Currently only the byte selection operator is supported, denoted `$`. The expression `N$M` is equal to the Mth byte of N, with 0 representing the least-significant byte; in this operation M must be a decimal number and cannot contain other symbols.

Symbols may be defined in one of two ways:
* Placing a label `name:` in the program sets the value of the symbol "name" to the address of the first instruction following the label.
* The macro `@define name <value>` sets the value of the symbol "name" to the given value. This assignment is not recursive; it may depend on any label or any `@define` before it in the program, but not itself or any `@define` after it.

Symbols may be used in most arithmetic and program-flow operations as if they were numbers. Symbols should only be defined using ASCII letters and underscores; defining a symbol using characters outside this set may function on some assembler versions but is undefined behavior.

## Using The NANDy Architecture

### Overview
NANDy is an accumulator-based architecture with four core registers and 16-bit memory addressing. It supports basic arithmetic operations on 8 bits of data, including addition, subtraction, and bitwise operators, as well as carry-in and carry-out for operating on larger datatypes.

### Registers
NANDy has four core registers, specified as follows:
* ACC: Accumulator; used for all mathematical operations; additionally is the data register for all memory operations.
* DX: Data X register; can be used as second operand in arithmetic operands; used as lower 8 bits for memory addresses (alias X, DL)
* DY: Data Y register; can be used as second operand in arithmetic operands; used as upper 8 bits for memory addresses (alias Y, DH)
* SP: Stack pointer; used as lower 8 bits of address in stack-addressing mode; incrementable/decrementable independently of ACC

The I/O bus is also treated as a register from the programmer's perspective and may be accessed by all register-move operations; however, it represents separate input and output ports, so no data written to it can be read back.

Registers are primarily manipulated via the `rd`, `wr`, and `sw` instructions, which read a register's value into the accumulator, write the accumulator's value into a register, and swap the accumulator with a register, respectively. There are no direct operations for manipulating non-accumulator registers relative to each other, but the same effect can be achieved by multiple swaps; for example, the nonexistent `sw dx, dy` is equivalent to:
```
sw dx
sw dy
sw dx
```
Constant values may be read into the accumulator via the `rdi` instruction. Similar to register moves, there is no way to directly read into registers other than the accumulator, but the same can be accomplished by a series of swaps:
```
sw dx
rdi 100
sw dx
```
For convenience, the included assembler provides a `move` macro that moves a value from any register to any other register while minimally disturbing other registers. This macro will expand to one or more `sw`, `rd`, and `wr` instructions when assembled; it is generally fairly efficient, but if some registers contain "don't care" values
better performance may sometimes be achieved by hand-writing the swap sequences.

### Arithmetic
NANDy provides a number of 8-bit arithmetic operations as primitive instructions. Most are two-operand; one input can be selected between DX and DY, and the other is always the accumulator, with the result placed back in the acccumulator. A few operations have only one operand, such as `sl` - these always act directly on the accumulator. The basic arithmetic operations are as follows:
* `add`: Adds the two operands
* `sub`: Subtracts the second operand from the accumulator
* `xor`: Bitwise-xors the two operands
* `and`: Bitwise-ands the two operands
* `or`: Bitwise-ors the two operands
* `sl`/`sr`: Shifts the accumulator left/right by 1 place, filling with zeroes
* `slr`/`srr`: Rotates the accumulator left/right by 1 place
* `sla`/`sra`: Shifts the accumulator left/right by 1 place, leaving the vacated position unchanged

By default, all operations except bitwise logical ones (`xor`, `and`, `or`) modify the value of the carry bit. For addition and subtraction, the carry bit is set to the value that the 9th bit of the result would take were it extended by an extra bit; for shifts, it is set to the value that was shifted off the end of the register. Operations may be prevented from setting the carry bit by prepending an underscore (`_`) to their mnemonic. There also exists a set of comparison operations which only modify the carry bit, and do not affect the accumulator at all:
* `ne`: Sets the carry bit to 1 if ACC is not equal to the second operand, 0 otherwise.
* `eq`: Sets the carry bit to 1 if ACC is equal to the second operand, 0 otherwise.
* `lt`: Sets the carry bit to 1 if ACC is less than the second operand, 0 otherwise.
* `ge`: Sets the carry bit to 1 if ACC is greater than or equal to the second operand, 0 otherwise.

Several instructions allow a suffix of `c` to indicate that the carry bit should be used as a mathematical carry; specifically, `add`, `sub`, `sl`, and `sr` can be modified to `addc`, `subc`, `slc`, and `src`. In addition and subtraction, this overrides the carry-in line of the adder; in shift operations, it specifies the value that should be shifted into the new place. This allows operations to be chained for larger bit widths, as in this example of a 16-bit left shift of the combined value of [DX, ACC]:
```
sl
sw dx
slc
sw dx
```
Two-operand arithmetic instructions may have `i` appended to their mnemonic, indicating that they operate on an immediate value instead of a register. These instructions take a signed 8-bit integer as their argument and otherwise behave the same as their register equivalents. Performance-minded users should be aware that these instructions use two clock cycles and two bytes of program memory, as opposed to register operations that take only one.

All modifiers can be combined, limited only by which operations each modifier is compatible with. For example, the instruction `_addci 3` adds the value 3 plus the carry bit to the accumulator without modifying the carry bit. Modifiers must be combined in the order shown - `addic` is not a valid instruction.

### Memory
NANDy uses a single memory address space for both program and data memory. Implementations may choose to use any memory layout they choose, however memory address 0 must be the start of program execution and memory address 0xFFFF must be writable; the reference implementation places program ROM between 0x0000 and 0x7FFF and general-purpose RAM between 0x8000 and 0xFFFF. Implementations which do not support a full 64KB of memory may choose to ignore upper bits of the address in order to satisfy these constraints, but these implementations may require modifications to the assembler in order to function correctly.

Memory may be addressed in one of two modes: absolute mode and stack mode. In absolute mode, as in the `lda` and `stra` instructions, DX and DY are combined to form a 16-bit base address and this address is further offset by the immediate value given. In stack mode, as in `lds` and `strs`, SP is combined with the upper byte 0xFF to form a base address and this address is similarly offset by the immediate. Immediates for these instructions are signed 4-bit values, allowing an offset of up to +7 or -8 bytes.

As an 8-bit architecture, NANDy has no hardware-defined endianness or byte alignment. In general, it is encouraged to use little-endian representations and single-byte alignment, as this provides the best combination of memory efficiency and compatibility with other architectures.

## Instruction Reference
