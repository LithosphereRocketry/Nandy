# NANDy Design Principles

## Overview of Design Principles
The NANDy architecture was built around several design principles:
1. Simplicity. I have not a lot of budget for this project and even less time to spend soldering hundreds of components. Therefore, the primary goal is to make this CPU as simple as possible.
2. Usability. While I could make a [Subleq](https://en.wikipedia.org/wiki/One-instruction_set_computer#Subtract_and_branch_if_less_than_or_equal_to_zero "Wikipedia: One-instruction set computer: Subleq") or [Brainfuck](https://en.wikipedia.org/wiki/Brainfuck "Wikipedia: Brainfuck") machine with potentially less components, these architectures are simplistic to the point of being painful to use for even basic tasks. Therefore, the ISA of this processor must be capable enough that someone might actually want to use it.
3. Performance. This is the least important design tenet to me; this is primarily a demonstration and not really designed to accomplish any particular task. However, it would be pretty neat if it could run at a respectable clockspeed.
## Architecture
  NANDy uses a simple Harvard accumulator architecture, with one accumulator register, one data register, and one stack pointer register. All mathematical operations place their result in the accumulator with the exception of stack-pointer increment instructions. Data memory and program memory each provide 256 bytes of total storage in separate address spaces.  
   
The architecture is implemented mostly as a single cycle, with all register reads continuous and all register writes on the same clock edge. Jumps are the only exception to this. All branch and jump operations use absolute addressing and use a two-cycle state machine to read jump addresses, with the byte following the jump instruction in memory determining the address of the jump.  
### ALU Structure
NANDy uses a single 8-bit ALU for all mathematical and logical operations. The operation performed by each operation is specified in the table below. In addition to its 8-bit output, the ALU outputs a single-bit status flag, `carry`. A unique meaning of this flag is given to each ALU instruction type:
| Instruction | Output            | Carry bit |
| ---         | ---               | ---       |
| add         | a + b             | carryout  |
| addc        | a + b + cin       | carryout  |
| sub         | a - b             | carryout  |
| subc        | a - b + !cin      | carryout  |
| lu          | b<<4 | a>>4       | a != b    |
| xor         | a ^ b             | a == b    |
| and         | a & b             | a > b     |
| or          | a \| b            | a <= b    |
| sl          | a << 1            | a[7]      |
| slc         | a << 1 \| cin     | a[7]      |
| slr         | a << 1 \| a[7]    | a[7]      |
| sla         | a << 1 \| a[0]    | a[7]      |
| sr          | a >> 1            | a[0]      |
| src         | a >> 1 \| cin     | a[0]      |
| srr         | a >> 1 \| a[0]<<7 | a[0]      |
| sra         | a >> 1 \| a[7]<<7 | a[0]      |