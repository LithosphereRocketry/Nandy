# `nandy-asm` Assembler Manual

## Introduction
This is a manual for the features of the reference `nandy-asm` assembler program
for the NANDy architecture. This document covers only features of the assembler
itself; for information on the architecture itself, consult the Programmer's
Guide and Instruction Reference documents.

## Theory of Operation
The NANDy reference assembler operates in two main passes: a layout phase and a
resolution phase.

### Layout Phase
Before parsing, the label `ISR` is set at the fixed location `0xFF00`.

During the layout phase, instructions are parsed from top to bottom and opcodes
placed in memory. Static and literal allocations are also determined at this
time, and in-program labels are determined as parsing reaches them.

During this phase, value expressions -- for example, labels and mathematical
expressions -- are evaluated opportunistically; they will only be evaluated if
all of their prerequisites are already evaluated, and otherwise they will be
deferred to the next phase.

By the end of this phase, the memory layout of the program must be fully
determined. In practice, this means that layout macros such as `@loc` or
`@static` can only depend on labels that occur before them in the program;
otherwise, it would be possible to make a layout which is impossible to resolve.

At the end of this phase, the label `FREE_MEM` is set at the first non-allocated
location in RAM.

### Resolution Phase
During the resolution phase, all instructions that were deferred in the layout
phase are re-parsed in top-to-bottom order. No further recursive processing is 
performed; if an expression is missing dependencies at this stage, assembly will
fail with an error. In general, this means it is most safe to have expressions
depend only on expressions defined above them in the code.

## Reference
### Supported Numeric Formats
All formats are case-insensitive.
| Format   | Meaning                    |
| -------- | -------------------------- |
| `1234`   | Decimal number             |
| `0x1234` | Hexadecimal number         |
| `0b1010` | Binary number              |
| `0o1234` | Octal number               |
| `'a'`    | ASCII character            |
| `'\n'`   | ASCII special character    |
| `'\''`   | ASCII escaped character    |
| `'\12'`  | Octal number               |
| `'\x12'` | Hexadecimal number         |
### Supported Operations
| Symbol | Operation                |
| ------ | ------------------------ |
| `+`    | Addition, unary identity |
| `-`    | Subtraction, negation    |
| `*`    | Multiplication           |
| `/`    | Integer division         |
| `%`    | Modulo                   |
| `&`    | Bitwise AND              |
| `\|`   | Bitwise OR               |
| `^`    | Bitwise XOR              |
| `~`    | Bitwise NOT              |
| `<<`   | Left shift               |
| `>>`   | Right shift              |
| `&&`   | Logical AND              |
| `\|\|` | Logical OR               |
| `!`    | Logical NOT              |
| `==`   | Equality                 |
| `!=`   | Inequality               |
| `>=`   | Greater than or equal    |
| `>`    | Greater than             |
| `<=`   | Less than or equal       |
| `<`    | Less than                |


### Macro Reference
#### Tooling Macros
These macros are closer to preprocessor calls in a conventional language: they
either do not produce any executable code or do so in an unconventional way.
To highlight this, they are all prefixed with "@" by convention.
##### `@echo <value>`
Prints the given value, along with the expression that generated it, to the
command line.
##### `@define <name> <value>`
Creates a symbol with the specified name and value.
##### `@assert <value>`
Causes program assembly to fail if the specified value is equal to zero.
##### `@static <name> <value>`
Reserves the given number of bytes of static RAM, and creates a label which
points to the start of the reserved block.
##### `@loc <value>`
Skips current assembly forward to a specified memory address. Contents of the
skipped region are undefined.
##### `@memloc <value>`
Skips static memory reservation forward to a specified memory address.
#### Code Macros
These macros generate finite chunks of code in the final executable, and as such
can be treated as conventional instructions; they do not follow the "@"
convetion.
##### `rda <label>`
Reads the specified label as a 16-bit value into DX and DY as an address.
##### `call <label>`
Calls the function at the specified label; stores return address in DX and DY.
##### `goto <label>`
Jumps to the specified label without a return address. Overwrites DX and DY.