# NANDy
Nandy (capitalization variable) is an 8-bit RISC computer architecture built for implementation using simple 74xx-series discrete NAND gates. It was inspired by the theorem shown in most digital logic classes that any piece of combinational logic may be recreated as a network of NAND gates.
## Design Principles
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
## Instruction set
### Program flow instructions
#### Jump instructions
`j <label>`  
Jumps to the instruction labeled by <label>. This instruction takes two clock cycles to complete and occupies two bytes in memory.  
`jsr <label>`  
Identical to `j`, except the current value of the program counter will be stored to the `data` register. This operation is most commonly used for calling procedures.  
`jif`  
Identical to `j`, except the jump will only be taken if the carry bit contains a 1. This will take two cycles to execute regardless of whether the branch is taken.  
`ret`  
Jumps to the address stored in `data`, plus one. Replaces the value in `data` with the current value of the program counter as with `jsr`. Unlike the other jump instructions, this instruction only takes a single cycle and single byte of ROM.  
#### Stack pointer manipulation instructions
`isp <number>`  
Increments the stack pointer by the given value (by convention, deallocates stack). Input is interpreted as an unsigned 4-bit integer.  
`dsp <number>`  
Decrements the stack pointer by the given value (by convention, allocates stack). Input is interpreted as an unsigned 4-bit integer.  
### Dataflow instructions
#### Memory store instructions
`stra <number>`, `stran <number>`  
Stores the value in `acc` at the address pointed to by `data` with offset `<number>`. Input is interpreted as an unsigned 3-bit integer, with the offset added for `stra` and subtracted for `stran`.  
`strs <number>`, `strsn <number>`  
Stores the value in `acc` at the address pointed to by `sp` with offset `<number>`. Input is interpreted as an unsigned 3-bit integer, with the offset added for `strs` and subtracted for `strsn`.  
#### Memory load instructions
`lda <number>`, `ldan <number>`  
Loads the value at the address pointed to by `data` with offset `<number>` into `acc`. Input is interpreted as an unsigned 3-bit integer, with the offset added for `lda` and subtracted for `ldan`.  
`lds <number>`, `ldsn <number>`  
Loads the value at the address pointed to by `sp` with offset `<number>` into `acc`. Input is interpreted as an unsigned 3-bit integer, with the offset added for `lds` and subtracted for `ldsn`.  
#### Register manipulation instructions
`wrd`, `wrs`  
Writes the value in `acc` to `data` (for `wrd`) or `sp` (for `wrs`).  
`swd`, `sws`  
Swaps the values of `acc` and `data` (for `swd`) or `acc` and `sp` (for `sws`).  
`rdd`, `rds`  
Reads the value in `data` (for `rdd`) or `sp` (for `rds`) into `acc`.
#### I/O instructions
`ioa`, `iob`  
Writes the value of `acc` to I/O output A or B, respectively. Also sets the value of `acc` to the input read from I/O input A or B, respectively.
### Math instructions
#### Addition instructions
`add`  
Adds the value of `data` to the value of `acc` and stores the result in `acc`; updates `carry` with the carry-out result of the addition.  
`addc`  
Identical to `add`, but additionally adds the value of the carry bit to the result. This is typically used for 16-bit addition.  
`addi <number>`  
Identical to `add`, but adds `<number>` to `acc` in place of `data`.  
`_add`, `_addc`  
Identical to `add` and `addc` respectively, but do not update the carry bit.  
`cry`  
Sets the carry bit to the carry result of `add`, but does not actually modify `acc`.  
`cryc`  
Sets the carry bit to the carry result of `addc`, but does not actually modify `acc`.  
#### Subtraction instructions
`sub`  
`subc`  
`subi`  
`_sub`, `_subc`  
`scry`  
`scryc`
#### Halfword manipulation instructions
`lu`  
`lui`  
`_lu`  
`ne`
#### XOR instructions
`xor`  
`xori`  
`_xor`  
`eq`  
#### AND instructions
`and`  
`andi`  
`_and`  
`gt`
#### OR instructions
`or`  
`ori`  
`_or`  
`le`  
#### Left-shift instructions
`sl`  
`slc`  
`slr`  
`sla`  
`_sl`, `_slc`, `_slr`, `_sla`  
`top`
#### Right-shift instructions
`sr`  
`src`  
`srr`  
`sra`  
`_sr`, `_src`, `_srr`, `_sra`  
`bot`
