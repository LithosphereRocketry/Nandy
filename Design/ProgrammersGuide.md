# NANDy Programmer's Manual

NOTE: implementation of these features not yet complete

## About This Guide
This guide is intended to detail all of the explicit rules, as well as some best
practices and implementation hints, for writing programs in NANDy assembly.
Because the NANDy is a very simple processor with limited resources, it may
often be necessary to deviate from these guidelines for best performance, so
this document can and should be ignored whenever it is necessary; however,
following these conventions will make code significantly easier to understand.  

Throughout this guide, `code markdown` is used for code snippets, instruction
specifications, and other pieces of useful text. In general, code markdown with
`<angle brackets>` is used to denote a required parameter, while
`[square brackets]` are used to denote optional syntax.

## The NANDy Assembler

### Basic Syntax
NANDy provides a reference assembler design which supports all core instructions
as well as a number of helpful shortcuts and macros, which are detailed in their
relevant sections throughout this guide. The basic structure of a line of
assembly is:
```
[<label>:] <command> [<operands>] [#<comment>]
```
It is also allowable to have a line with only a label, only a comment, or no
content at all. Arguments are whitespace-delineated, and it is not necessary to
have any whitespace after a label or before a comment; all whitespace other than
newlines is considered equivalent.

By convention, commands starting with `@` represent preprocessing macros that
expand to either nothing or complex external constructs; other assemblers are
very strongly discouraged (but not prohibited) from using the `@` character in
any other context.

### Literals and Symbols
Several instructions take a literal value as an argument; this can be a decimal
number *(more radices coming soon) or a combination of operations on another
literal. Currently only the byte selection operator is supported, denoted `$`.
The expression `N$M` is equal to the Mth byte of N, with 0 representing the
least-significant byte; in this operation M must be a decimal number and cannot
contain other symbols.

Symbols may be defined in one of three ways:
* Placing a label `name:` in the program sets the value of the symbol "name" to
the address of the first instruction following the label.
* The macro `@define name <value>` sets the value of the symbol "name" to the
given value. This assignment is not recursive; it may depend on any label or any
`@define` before it in the program, but not itself or any `@define` after it.
* The macro `@static <amount> name` reserves `amount` bytes at the bottom of RAM
for static variables, and creates a symbol `name` containing the address of the
first byte of the reserved region.

Additionally, a predefined symbol `FREE_MEM` is provided pointing to the first
byte of RAM not reserved by `@static`, and a predefined symbol `ISR` is provided
with the value 0x7F00, corresponding to the address of the interrupt service
routine.

Symbols may be used in most arithmetic and program-flow operations as if they
were numbers. Symbols should only be defined using ASCII letters and
underscores; defining a symbol using characters outside this set may function on
some assembler versions but is undefined behavior.

## Using The NANDy Architecture

### Overview
NANDy is an accumulator-based architecture with four core registers and 16-bit
memory addressing. It supports basic arithmetic operations on 8 bits of data,
including addition, subtraction, and bitwise operators, as well as carry-in and
carry-out for operating on larger datatypes.

### Registers
NANDy has four core registers, specified as follows:
* ACC: Accumulator; used for all mathematical operations; additionally is the
data register for all memory operations
* DX: Data X register; can be used as second operand in arithmetic operands;
used as lower 8 bits for memory addresses (alias X, DL)
* DY: Data Y register; can be used as second operand in arithmetic operands;
used as upper 8 bits for memory addresses (alias Y, DH)
* SP: Stack pointer; used as lower 8 bits of address in stack-addressing mode;
incrementable/decrementable independently of ACC

The I/O bus is also treated as a register from the programmer's perspective and
may be accessed by all register-move operations; however, it represents separate
input and output ports, so no data written to it can be read back.

Registers are primarily manipulated via the `rd`, `wr`, and `sw` instructions,
which read a register's value into the accumulator, write the accumulator's
value into a register, and swap the accumulator with a register, respectively.
There are no direct operations for manipulating non-accumulator registers
relative to each other, but the same effect can be achieved by multiple swaps;
for example, the nonexistent `sw dx, dy` is equivalent to:
```
sw dx
sw dy
sw dx
```
Constant values may be read into the accumulator via the `rdi` instruction.
Similar to register moves, there is no way to directly read into registers other
than the accumulator, but the same can be accomplished by a series of swaps:
```
sw dx
rdi 100
sw dx
```
For convenience, the included assembler provides a `move` macro that moves a
value from any register to any other register while minimally disturbing other
registers. This macro will expand to one or more `sw`, `rd`, and `wr`
instructions when assembled; it is generally fairly efficient, but if some
registers contain "don't care" values better performance may sometimes be
achieved by hand-writing the swap sequences.

### Arithmetic
NANDy provides a number of 8-bit arithmetic operations as primitive
instructions. Most are two-operand; one input is always the accumulator,
and the other can be selected between DX and DY, with the result placed back in
the acccumulator. A few operations have only one operand, such as `sl` - these
always act directly on the accumulator. The basic arithmetic operations are as
follows:

* `add`: Adds the two operands
* `sub`: Subtracts the second operand from the accumulator
* `xor`: Bitwise-xors the two operands
* `and`: Bitwise-ands the two operands
* `or`: Bitwise-ors the two operands
* `xnor`: Bitwise-xnors the two operands
* `nand`: Bitwise-nands the two operands
* `nor`: Bitwise-nors the two operands
* `inv`: Sets accumulator equal to the bitwise inverse of the second operand (*)
* `sl`/`sr`: Shifts the accumulator left/right by 1 place, filling with zeroes
* `slr`/`srr`: Rotates the accumulator left/right by 1 place
* `sla`/`sra`: Shifts the accumulator left/right by 1 place, leaving the vacated
position unchanged

(*) If this instruction seems like a bit of an anomaly, it is - it is more a
consequence of spare logic in the ALU and not really needed for any particular
purpose. However, since it technically fulfils a unique purpose, it is
included here rather than left undocumented.

By default, all operations except bitwise logical ones (`xor`, `and`, `or`)
modify the value of the carry bit. For addition and subtraction, the carry bit
is set to the value that the 9th bit of the result would take were it extended
by an extra bit; for shifts, it is set to the value that was shifted off the end
of the register. Operations may be prevented from setting the carry bit by
prepending an underscore (`_`) to their mnemonic. There also exists a set of
operations which only modify the carry bit, and do not affect the accumulator at
all:
* `zero`: Set the carry bit high if the accumulator is zero, and low otherwise.
* `nzero`: Inverse of `zero`.
* `par`: Set the carry bit high if the accumulator contains an odd number of
high bits, and low otherwise.
* `npar`: Inverse of `par`. 
* `cset`: Set the carry bit high unconditionally.
* `cclr`: Set the carry bit low unconditionally.
* `ctog`: Invert the value of the carry bit.

Several instructions allow a suffix of `c` to indicate that the carry bit should
be used as a mathematical carry; specifically, `add`, `sub`, `sl`, and `sr` can
be modified to `addc`, `subc`, `slc`, and `src`. In addition and subtraction,
this overrides the carry-in line of the adder; in shift operations, it specifies
the value that should be shifted into the new place. This allows operations to
be chained for larger bit widths, as in this example of a 16-bit left shift of
the combined value of [DX, ACC]:
```
sl
sw dx
slc
sw dx
```
Two-operand arithmetic instructions may have `i` appended to their mnemonic,
indicating that they operate on an immediate value instead of a register. These
instructions take a signed 8-bit integer as their argument and otherwise behave
the same as their register equivalents. Performance-minded users should be aware
that these instructions use two clock cycles and two bytes of program memory, as
opposed to register operations that take only one.

All modifiers can be combined, limited only by which operations each modifier is
compatible with. For example, the instruction `_addci 3` adds the value 3 plus
the carry bit to the accumulator without modifying the carry bit. Modifiers must
be combined in the order shown - `addic` is not a valid instruction.

### Memory
NANDy uses a single memory address space for both program and data memory.
Implementations may choose to use any memory layout they choose, however memory
address 0 must be the start of program execution and memory address 0xFFFF must
be writable; the reference implementation places program ROM between 0x0000 and
0x7FFF and general-purpose RAM between 0x8000 and 0xFFFF. Implementations which
do not support a full 64KB of memory may choose to ignore upper bits of the
address in order to satisfy these constraints, but these implementations may
require modifications to the assembler in order to function correctly.

Memory may be addressed in one of two modes: absolute mode and stack mode. In
absolute mode, as in the `lda` and `stra` instructions, DX and DY are combined
to form a 16-bit base address and this address is further offset by the
immediate value given. In stack mode, as in `lds` and `strs`, SP is combined
with the upper byte 0xFF to form a base address and this address is similarly
offset by the immediate. Immediates for these instructions are unsigned 4-bit
values, allowing a positive offset of 0 to 15 bytes.

Implementations will typically designate some portion of memory as "read-only"
for the purposes of boot or program ROM. In the reference implementation,
read-only memory extends from 0x0000 to 0x7FFF. Writing to read-only memory is
undefined behavior, and may result in corruption of memory distant from the
address written.

As an 8-bit architecture, NANDy has no hardware-defined endianness or byte
alignment. In general, it is encouraged to use little-endian representations and
single-byte alignment, as this provides the best combination of memory
efficiency and compatibility with other architectures.

#### The Stack
NANDy has a 256-byte stack, located betweeen addresses 0xFF00 and 0xFFFF. By
convention the stack grows downwards; the stack pointer should be initialized to
0x00 and then decremented to allocate space. In order to improve performance, an
instruction `isp` (and corresponding no-carry version `_isp`) is provided which
moves the stack pointer by the provided 4-bit immediate in a single cycle.

Generally speaking, memory addresses below the stack pointer are considered to
be undefined in the context of stack access, and it should be expected that
their contents may change at any time. Reading off the top of the stack (offset
above 0xFFFF) is also undefined behavior, and will usually result in reading the
first bytes of program memory.

In some cases, it may be necessary or advantageous to neglect the stack memory
region and instead use the stack pointer as an extra general-purpose register.
This is referred to as "stack-pointer abuse." Anytime the stack pointer does not
represent the lowest allocated memory address on the stack, whether due to abuse
or non-initialization, the stack is said to be "invalid"; when that condition is
true, it is said to be "valid." 

### Program Flow
NANDy program flow is provided by a small handful of jump instructions. Local
conditional jumps are handled by the `jif` and `jnif` instructions. Both
take the form `j[n]if <label>`, where the label may also be replaced by a direct
memory address, but this practice is not recommended. These jumps have a range
limit of [-2048, +2047] bytes; attempting to jump outside this range will cause
an error in assembly. `jif` will jump to the specified label if the specified
signal is high, while `jnif` will jump if the specified signal is low. There is
no unconditional local jump; however, the `j` macro is provided that fulfils the
same function, equivalent to `jif` followed by `jnif`.

Of note for optimization is that relative jump instructions will take two
cycles - this includes when the jump is not taken. Therefore it is usually
faster to let a loop fall through when it is finished. The `j` macro is
especially disadvantageous for performance-sensitive code; it may take either 2
or 4 cycles depending on the state of the carry bit.
```
loop:
    # this is fast
    ...
    jnif loop
    # done
```
```
loop:
    # this is slow
    ...
    jif done
    j loop
done: # done
```
For jumps outside relative range, the absolute jump instructions `ja` and `jar`
must be used. Both jump to the address formed by the combined Y and X registers;
`ja` leaves the registers unchanged, while `jar` replaces the register values
with the address of the instruction following the origin of the jump. `ja` and
`jar` take no arguments; the assembler provides the macros `goto <label>` and
`call <label>`, which automate the placement of address values in registers for
each respectively. Both macros leave the accumulator unchanged; neither retains
the previous values of X and Y anywhere in memory.

#### Function Calling
Function calling is typically accomplished with the `call` macro, along with use
of the stack. The following conventions apply:

* The stack must be valid at the time of call
* Return address is passed in the X and Y registers
* Functions must return the stack pointer to its original state
* Arguments are passed in the accumulator and on the stack
* Return values are passed in the accumulator and on the stack
* At return, the stack pointer should be returned to its original state

Upon call, a function "claims" a certain amount of stack space, which should be
carefully documented as it may not be obvious. This is distinct from allocating
stack space and is used for parameter passing. For example, a function that
multiplies two 8-bit values might claim one byte of stack, pass its arguments in
the accumulator and claimed byte, and return a 16-bit value split across the
accumulator and claimed byte. Any claimed value that does not have an assigned
meaning (typically, because padding is required to maintain the same stack
pointer location) should be treated as "don't care"; if it has no meaning as an
argument, it should accept any value, and if it has no meaning as a return, it
may return containing any value.

### Input/Output
I/O on the NANDy is handled by a single 8-bit parallel I/O port. Data may be
written to and read from the port using standard register-move instructions;
`rd io` retrieves data from the input port, `wr io` writes it to the output
port, and `sw io` performs both simultaneously. Other devices may interface with
the I/O port via the signal pins `IOWrite` and `IORead`; a positive pulse is
transmitted on `IOWrite` whenever data is written to the I/O port and on
`IORead` whenever data is read from the I/O port.

#### Interrupts
For real-time operations, NANDy provides a single interrrupt service routine.
To allow interrupts, they must first be enabled using the instruction `eint`;
they can later be disabled with the instruction `dint`. Interrupts must only be
enabled while the stack is valid. An interrupt will be triggered when the
following conditions are met:

* The CPU is about to execute an instruction (i.e. not halfway through a
multicycle operation)
* Interrupts are enabled
* An interrupt service routine is not currently occurring
* The INT pin is high
* The INT pin was low last time an instruction was executed

Upon triggering an interrupt, the CPU exchanges the normal DX and DY registers
for alternate registers used only for interrupts, populates those registers with
the intended next instruction, and then jumps to the adress 0x7F00, denoted by
the label ISR. Execution continues from that point until a `jri` instruction is
encountered. This instruction behaves identically to `ja` except that it also
exits ISR mode, restoring DX and DY to their normal state and resuming the
processing of new interrupts.

A simple program which repeatedly outputs the letter 'a' and plays a bell sound
whenever an interrupt is registered might look like this:
```
    rdi 0
    wr sp
    eint
    rdi 65
idle:
    wr io
    j idle

@loc ISR
    bell
    jri
```
Interrupts follow similar calling convetions to functions except that they must
leave all registers unmodified, including the carry bit. The alternate registers
are interfaced exactly as DX and DY are in normal program execution. Currently,
there is no way to access the alternate registers except during an interrupt,
but this may change in future extensions of the architecture.

## Instruction Reference
