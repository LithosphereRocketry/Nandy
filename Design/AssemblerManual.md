# `nandy-asm` Assembler Manual

## Introduction
This is a manual for the features of the reference `nandy-asm` assembler program
for the NANDy architecture. This document covers only features of the assembler
itself; for information on the architecture itself, consult the Programmer's
Guide and Instruction Reference documents.

## Theory of Operation


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
##### `@
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