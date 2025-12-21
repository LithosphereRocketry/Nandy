# NANDy Programmer's Manual

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
Several instructions take a literal value as an argument; this can be any
mathematical expression of numeric literals and symbols. Generally speaking,
assignment of symbols is not recursive; more detail may be found in Appendix B. 
Placing a label `name:` in the program sets the value of the symbol "name" to
the address of the first instruction following the label; other methods of using
labels are detailed in the Assembler Manual.

Additionally, a predefined symbol `FREE_MEM` is provided pointing to the first
byte of RAM not reserved by static allocation, and a predefined symbol `ISR` is
provided with the value 0x7F00, corresponding to the address of the interrupt
service routine.

Symbols may be used in most arithmetic and program-flow operations as if they
were numbers. Symbols should only be defined using ASCII letters, digits
after the first character, and underscores; defining a symbol using characters
outside this set may function on some assembler versions but is undefined
behavior.

## Using The NANDy Architecture

### Overview
NANDy is an accumulator-based architecture with six core registers and 16-bit
memory addressing. It supports basic arithmetic operations on 8 bits of data,
including addition, subtraction, and bitwise operators, as well as carry-in and
carry-out for operating on larger datatypes.

### Registers
NANDy has six core registers, specified as follows:
* ACC: Accumulator; used for all mathematical operations; additionally is the
data register for all memory operations
* X: Data X register; can be used as second operand in arithmetic operands
* Y: Data Y register; can be used as second operand in arithmetic operands;
    also used as address to I/O peripherals
* P: Primary pointer register; used as long jump target and function return
    address; supports memory addressing with post-increment
* Q: Secondary pointer register; supports only basic memory addresssing
* SP: Stack pointer; used as lower 8 bits of address in stack-addressing mode

The I/O bus is also treated as a register from the programmer's perspective and
may be accessed by all register-move operations; however, it represents separate
input and output ports, so no data written to it can be read back.

Registers are primarily manipulated via the `rd`, `wr`, and `sw` instructions,
which read a register's value into the accumulator, write the accumulator's
value into a register, and swap the accumulator with a register, respectively.
There are no direct operations for manipulating non-accumulator registers
relative to each other, but the same effect can be achieved by multiple swaps;
for example, swapping registers `x` and `y` is equivalent to this sequence:
```
sw x
sw y
sw x
```

The 16-bit pointer registers `p` and `q` are addressed by their high and low
bytes `ph`, `pl`, `qh`, and `ql`.

Constant values may be read into the accumulator via the `rdi` instruction.
Similar to register moves, there is no way to directly read into registers other
than the accumulator, but the same can be accomplished by a series of swaps:
```
sw x
rdi 100
sw x
```
For convenience, the included assembler provides a `move` macro that moves a
value from any register to any other register while minimally disturbing other
registers. This macro will expand to one or more `sw`, `rd`, and `wr`
instructions when assembled; it is generally fairly efficient, but if some
registers contain "don't care" values better performance may sometimes be
achieved by hand-writing the swap sequences. Similarly, it provides a `swap`
macro that exchanges any two registers without disturbing other registers.

Another common action is to read an absolute address into the entire `p` or `q`
register. The assembler provites the macros `rdp <label>` and `rdq <label>`,
which load the given 16-bit label value into `p` or `q` without disturbing the
accumulator.

### Arithmetic
NANDy provides a number of 8-bit arithmetic and logic operations as primitive
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
* `sl`/`sr`: Shifts the accumulator left/right by 1 place, filling with zeroes

By default, no operations modify the value of the carry bit. The arithemetic and
shift operations support a carry form `adc`/`sbc`/`slc`/`src`, which both takes
the carry bit as a carry input and stores the resulting carry output. For
addition and subtraction, the carry bit is set to the value that the 9th bit of
the result would take were it extended by an extra bit; for shifts, it is set to
the value that was shifted off the end of the register.

There also exists a set of operations which only modify the carry bit, and do
not affect the accumulator at all:
* `cclr`: Sets the carry bit low.
* `cset`: Sets the carry bit high.
* `zero`: Set the carry bit high if the accumulator is zero, and low otherwise.
* `nzero`: Inverse of `zero`.
* `sgn`: Set the carry bit to the upper (two's complement sign) bit of the
    accumulator.
* `nsgn`: Inverse of `sgn`. 
* `ctog`: Invert the value of the carry bit.

Two-operand arithmetic instructions may have `i` appended to their mnemonic,
indicating that they operate on an immediate value instead of a register. These
instructions take a signed 8-bit integer as their argument and otherwise behave
the same as their register equivalents. Performance-minded users should be aware
that these instructions use two clock cycles and two bytes of program memory, as
opposed to register operations that take only one.

### Memory
NANDy uses a single memory address space for both program and data memory.
Implementations may choose to use any memory layout they choose, however memory
address 0 must be the start of program execution, address 0x7F00 must be the
location of interrupt routines, and memory address 0xFFFF must be writable; the
reference implementation places program ROM between 0x0000 and 0x7FFF and
general-purpose RAM between 0x8000 and 0xFFFF. Implementations which do not
support a full 64KB of memory may choose to ignore upper bits of the address in
order to satisfy these constraints, but these implementations may require
modifications to the assembler in order to function correctly.

Memory may be addressed in one of two four modes, each accepting an immediate
offset. Immediates for these instructions are unsigned 4-bit values, allowing a
positive offset of 0 to 15 bytes.

* In stack mode (`sp`), the base address is formed from (0xFE00 | `sp`) or
    (0xFF00 | `sp`) depending on whether the CPU is currently in an interrupt
    routine.
* In P mode (`p`), the base address is given by `p`.
* In Q mode (`q`), the base address is given by `q`.
* In P-postincrement mode (`p+`), the base address is given by `p` and the
    offset is not applied to the address. Instead, `p` is incremented by the
    offset after the access is completed.

Implementations will typically designate some portion of memory as "read-only"
for the purposes of boot or program ROM. In the reference implementation,
read-only memory extends from 0x0000 to 0x7FFF. Writing to read-only memory is
undefined behavior, and may result in corruption of memory distant from the
address written.

As an 8-bit architecture, NANDy has no hardware-defined endianness or byte
alignment. In general, it is encouraged to use little-endian representations;
usefulness of word-alignment varies depending on the application.

#### The Stack
NANDy has a 256-byte stack, located betweeen addresses 0xFF00 and 0xFFFF. By
convention the stack grows downwards; the stack pointer should be initialized to
0x00 and then decremented to allocate space. In order to improve performance, an
instruction `isp` (and corresponding no-carry version `_isp`) is provided which
moves the stack pointer by the provided 4-bit immediate in a single cycle.

Generally speaking, memory addresses below the stack pointer are considered to
be undefined in the context of stack access, and it should be expected that
their contents may change at any time. Reading off the top of the stack (offset
above 0xFEFF or 0xFFFF) is also undefined behavior, and will usually result in
reading garbage left by a previous interrupt or the first bytes of program
memory.

Like other registers, the stack pointer has no guaranteed initial value.
Therefore, it must be initialized before the stack can be used.

### Program Flow
NANDy program flow is provided by a handful of jump instructions. Local
jumps are handled by the `j`, `jr`, `jc`, and `jcr` instructions. All take the
form `j[c][r] <label>`, where the label may also be replaced by a direct memory
address; however, using direct addresses is not recommended. These jumps have a
range limit of [-1024, +1023] bytes; attempting to jump outside this range will
cause an error in assembly. If the `r` flag is included, the jump will store its
source address in `p` to allow returning from a function call; if the `c` flag
is included, the jump will only be taken if the carry flag is set.

Of note for optimization is that relative jump instructions will always take two
cycles - this includes when the jump is not taken. Therefore it is usually
faster to let a loop fall through when it is finished, rather than breaking out
of it when a condition is met:
```
loop:
    # this is fast
    ...
    nzero
    jc loop
    # done
```
```
loop:
    # this is slow
    ...
    zero
    jc done
    j loop
done: # done
```
For jumps outside relative range, the absolute jump instructions `jp` and `jpr`
must be used. Both jump to the address in the `p` register; like `jr`, `jpr`
stores the current address in `p` to allow function returns. `jp` and `jpr` take
no arguments; the assembler provides the macros `goto <label>` and
`call <label>`, which automate the placement of address values in registers for
each respectively. Both macros leave the accumulator unchanged; neither retains
the previous value of `p` anywhere in memory.

#### Function Calling
Function calling is typically accomplished with `jr` or the `call` macro, along
with use of the stack. Which registers are saved versus clobbered and which are
used as arguments are not specified, except that `p` is always clobbered by the
return address. Programmers should carefully document the behavior of each
function. It is encouraged that functions return the stack pointer to its
original state but it is not required.

### Input/Output
I/O on the NANDy is handled by a single 8-bit parallel I/O port. Data may be
written to and read from the port using standard register-move instructions;
`rd io` retrieves data from the input port, `wr io` writes it to the output
port, and `sw io` performs both simultaneously, although most devices do not
support this. Other devices may interface with the I/O port via the signal pins
`IOWrite` and `IORead`; a positive pulse is transmitted on `IOWrite` whenever
data is written to the I/O port and on `IORead` whenever data is read from the
I/O port.

I/O devices are also provided with the value of the `y` register to use as an
address.

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
* An IO device has asserted the INT signal

Upon triggering an interrupt, the CPU stores the current PC in the hidden IA
register and then jumps to the adress 0x7F00, denoted by the label ISR.
Execution continues from that point until a `jxi` instruction is
encountered. This instruction restores PC from IA and clears the interrupt
status flag.

A simple program which echoes incoming values from the input back to the output
might look like this (note that real IO devices may require more operations than
a single write):

```
    rdi 0
    wr y
    wr sp
    eint
idle:
    j idle

@loc ISR
    rd io
    wr io
    jxi
```
Note that interrupts do not have any debouncing or hysteresis; if the interrupt
line is held high, interrupts will continue to occur until the interrupt
stimulus disappears. It is the responsibility of peripheral devices to allow
their interrupt status to be cleared via some I/O interaction, which may differ
on a per-device basis.

Interrupts must leave all registers unmodified, including the carry bit. They
have their own stack region, but not their own stack pointer; this creates a
slight complication in allocating stack space in interrupts. If the stack
pointer given to an ISR is very close to 0, decrementing may cause it to wrap
to the top of the stack space, causing subsequent positive offsets to offset
past the top of the stack.

This is an example of a correct ISR handler which ensures a reasonable amount
of stack space:

```
@loc ISR
    st sp 0 # stash accumulator at current SP (no offset is always safe)
    isp -1 # allocate one more byte
    slc
    st sp 0 # stash carry bit in bottom bit of next byte

    isp -1
    # figure out whether our first two stashes overlap the top of the stack
    # where we want our stack to start; if SP is in the top half we move it
    rd sp
    nsgn
    # if the top bit is 1 (carry 0), start at 0x7F; otherwise start at 0xFF
    rdi 0x7F
    jc isr_low_stack
    rdi 0xFF
isr_low_stack:
    sw sp
    # store the old stack pointer at the top of our new stack
    st sp 0    
    ...
    # do ISR things, free to use up to about 125 bytes of stack without fear
    ...
    # restore the stack to its old position
    ld sp 0
    wr sp
    ld sp 0
    src # restore carry bit
    isp 1
    ld sp 0 # restore acc
    jxi # exit interrupt
```