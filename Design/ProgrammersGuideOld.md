# NANDy Programmer's Manual

NOTE: implementation of these features not yet complete

## About This Guide
This guide is intended to detail all of the explicit rules, as well as some best practices and implementation hints, for writing programs in NANDy assembly. Because the NANDy is a very simple processor with limited resources, it may often be necessary to deviate from these guidelines for best performance, so in no way should every word of this document be taken as gospel; however, following these conventions will make code significantly easier to understand.  

Throughout this guide, `code markdown` is used for code snippets, instruction specifications, and other pieces of useful text. In general, code markdown with `<angle brackets>` is used to denote a required parameter, while `[square brackets]` are used to denote optional syntax.  

## Common Constructs

## Optimization and "Hacks"
### Efficient Loops
A major part of optimizing NANDy programs, like in many architectures, is minimizing the number of jump instructions. In NANDy this is especially important, as static jumps take two cycles, regardless of whether the jump is conditional or whether a conditional jump actually executes. Take the following loop structures:
```
loop1:
	# code goes here
	jif end
	j loop1

loop2:
	# code goes here
	jnif loop2
	j end
	
end:
```
Both have identical functionality, but because the first loop executes `jif end` every cycle, it will have an additional two cycles of overhead for every loop cycle. Therefore, when designing loops, it is important to check for the most common condition first.

### Stack-Pointer Abuse
While the NANDy only offers two arithmetic registers, with convention being to use the stack for any additional parameters, many functions are much easier to implement if a third register is available. Take, for example, this pseudocode function to compute the Nth Fibonacci number:
```
fibonacci(n) {
    a = 0
    b = 1
    while(n > 0) {
        a += b
        swap a, b
        n --
    }
    return a
}
```
By typical NANDy calling conventions, this program could be implemented as:
```
fibonacci:
    dsp 3
    swd
    strs 2
    lui 1
    lui 0
    swd             # b = 1
    lt              # if count < 1, jump to return 0
    jif fibonacci_rz
    subi 1          # easier to measure < 0 than <= 0
    strs 0
    andi 0          # a = 0
fibonacci_loop:
    add             # a += b
    swd             # swap a, b
    strs 1
    lds 0
    subi 1          # n --
    strs 0
    lds 1           # carry flag from 'subi 1' persists through memory exchange
    jnif fibonacci_loop
    j fibonacci_end
fibonacci_rz:
    andi 0          # if we got here through the n<1 case, force 0
fibonacci_end:
    swd
    lds 2
    swd
    isp 3
    ret
```
However, this program leaves something to be desired in terms of efficiency. In particular, it swaps variables to and from the stack very frequently. A way to avoid this would be to use the stack-pointer register as the loop counter instead of swapping the value to and from memory:
```
# Memory address 0 statically allocated for SP abuse
fibonacci:
	dsp 1
	swd
    lui 1
    lui 0
    swd  			# b = 1
    lt              # if count < 1, jump to return 0
    jif fibonacci_rz
    subi 1          # easier to measure < 0 than <= 0
	sws				# this is where the magic starts
	stran 1			# store at address 1-1=0
	andi 0			# a = 0
fibonacci_loop:
    add             # a += b
    swd             # swap a, b
	dsp 1			# n --
    jnif fibonacci_loop
    j fibonacci_end
fibonacci_rz:
    andi 0          # if we got here through the n<1 case, force 0
fibonacci_end:
	sws
	andi 0
	swd
	lda	0			# retrieve old SP from address 0
	sws
	swd
	lds	0			# retrieve return address from stack and everything is right in the world again
	swd
	ret
```
Here we have sacrificed a byte of memory, along with some additional setup and teardown time, in order to significantly trim our loop time from 9 cycles to only 5. This strategy is denoted **SP abuse** or **stack abuse**, and may be used wihtin NANDy "good practice." However, it is subject to several caveats:
- The programmer is responsible for ensuring that the location used to store the stack pointer is not in use by any other routine.
- While in SP abuse, the stack is obviously not available. This means that all memory used must be statically allocated; both `strs` and `stra` will effectively be absolute writes, with different base addresses.
- Because the state of the stack is unknown, SP-abuse routines may not call other functions or call themselves recursively.

## Assembly Syntax Rules
### Assembly Files
By default, NANDy assembly programs are stored as plain 7-bit ASCII text files with the file extension `.na`.

### Instruction Formatting
Normal assembly instructions follow the following format:  
`[<label>:] <mnemonic> [<arguments>]`  
Labels may be any sequence of characters not including a colon or '@', and any amount of whitespace, but not a newline, may separate each argument. Labels should always be interpreted as case-sensitive; instruction mnemonics should always be valid in all lowercase, though implementations may choose to recognize case-insensitive mnemonics.

### Preprocessor
Preprocessor directives are defined by lines beginning with `@`.

## Instruction Set
### Program flow instructions
#### Debugging instructions
##### `nop`
Does nothing.
##### `brk`
Halts program execution until a hardware resume or hardware reset input is received.

#### Jump instructions
##### `j <label>`  
Jumps to the instruction labeled by <label>. This instruction takes two clock cycles to complete and occupies two bytes in memory.  
##### `jsr <label>`  
Identical to `j`, except the current value of the program counter will be stored to the `data` register. This operation is most commonly used for calling procedures.  
##### `jif`  
Identical to `j`, except the jump will only be taken if the carry bit contains a 1. This will take two cycles to execute regardless of whether the branch is taken.  
##### `jnif`  
Identical to `j`, except the jump will only be taken if the carry bit contains a 0. This will take two cycles to execute regardless of whether the branch is taken.  
##### `ret`  
Jumps to the address stored in `data`, plus one. Unlike the other jump instructions, this instruction only takes a single cycle and single byte of ROM.  
##### `retsr`  
Identical to `ret`, but replaces the value in `data` with the current value of the program counter as with `jsr`.

#### Stack pointer manipulation instructions
##### `isp <number>`  
Increments the stack pointer by the given value (by convention, deallocates stack). Input is interpreted as an unsigned 4-bit integer.  
##### `dsp <number>`  
Decrements the stack pointer by the given value (by convention, allocates stack). Input is interpreted as an unsigned 4-bit integer.  
### Dataflow instructions
#### Memory store instructions
##### `stra <number>`, `stran <number>`  
Stores the value in `acc` at the address pointed to by `data` with offset `<number>`. Input is interpreted as an unsigned 3-bit integer, with the offset added for `stra` and subtracted for `stran`.  
##### `strs <number>`, `strsn <number>`  
Stores the value in `acc` at the address pointed to by `sp` with offset `<number>`. Input is interpreted as an unsigned 3-bit integer, with the offset added for `strs` and subtracted for `strsn`.  
#### Memory load instructions
##### `lda <number>`, `ldan <number>`  
Loads the value at the address pointed to by `data` with offset `<number>` into `acc`. Input is interpreted as an unsigned 3-bit integer, with the offset added for `lda` and subtracted for `ldan`.  
##### `lds <number>`, `ldsn <number>`  
Loads the value at the address pointed to by `sp` with offset `<number>` into `acc`. Input is interpreted as an unsigned 3-bit integer, with the offset added for `lds` and subtracted for `ldsn`.  

#### Register manipulation instructions
##### `wrd`, `wrs`  
Writes the value in `acc` to `data` (for `wrd`) or `sp` (for `wrs`).  
##### `swd`, `sws`  
Swaps the values of `acc` and `data` (for `swd`) or `acc` and `sp` (for `sws`).  
##### `rdd`, `rds`  
Reads the value in `data` (for `rdd`) or `sp` (for `rds`) into `acc`.
#### I/O instructions
##### `ioa`, `iob`  
Writes the value of `acc` to I/O output A or B, respectively. Also sets the value of `acc` to the input read from I/O input A or B, respectively.
### Math instructions
#### Addition instructions
##### `add`  
Adds the value of `data` to the value of `acc` and stores the result in `acc`; updates `carry` with the carry-out result of the addition.  
##### `addc`
Identical to `add`, but additionally adds the value of the carry bit to the result. This is typically used for 16-bit addition.  
##### `addi <number>`  
Identical to `add`, but adds `<number>` to `acc` in place of `data`.  
##### `_add`, `_addc`  
Identical to `add` and `addc` respectively, but do not update the carry bit.  
##### `cry`  
Sets the carry bit to the carry result of `add`, but does not actually modify `acc`.  
##### `cryc`  
Sets the carry bit to the carry result of `addc`, but does not actually modify `acc`.  
#### Subtraction instructions
##### `sub`
Subtractes the value of `data` from the value of `acc` and stores the result in `acc`; updates `carry` with the carry-out result of the subtraction.
##### `subc`  
Identical to `sub`, but replaces the carry-in of the subtraction with the contents of the carry bit.
##### `subi <number>`  
Identical to `sub`, but subtracts `<number>` from `acc` in place of `data`.
##### `_sub`, `_subc`  
Identical to `sub` and `subc` respectively, but do not update the carry bit.
##### `scry`  
Sets the carry bit to the carry result of `sub`, but does not actually modify `acc`. Note that this may be used as an unsigned comparison operator.
##### `scryc`
Sets the carry bit to the carry result of `subc`, but does not actually modify `acc`.
#### Comparison instructions
##### `ne`
Sets `carry` to the result of `acc != data`. 
##### `eq`  
Sets `carry` to the result of `acc == data`.
##### `lt`
Sets `carry` to the result of `acc < data` via two's complement.
##### `ge`
Sets `carry` to the result of `acc >= data` via two's complement.
##### `top`
Sets `carry` to the top bit of `acc`. Note that this may be used to test if `acc` is negative.
##### `bot`
Sets `carry` to the bottom bit of `acc`. Note that this may be used to test if `acc` is even.
#### Halfword manipulation instructions
##### `lu`  
Moves the top 4 bits of `acc` into the bottom 4 bits of `acc`, and replaces the top 4 bits of `acc` with the bottom 4 bits of `data`. Also sets `carry` to the result of `acc != data`. 
##### `lui <number>`  
Moves the top 4 bits of `acc` into the bottom 4 bits of `acc`, and replaces the top 4 bits of `acc` with the bottom 4 bits of `<number>`. Also sets `carry` to the result of `acc != <number>`. Note that two `lui` instructions may be used to load an exact 8-bit value in `acc`.
##### `_lu`  
Identical to `lu`, but does not update the carry bit.
#### XOR instructions
##### `xor`  
Sets `acc` to a bitwise XOR of `acc` and `data`. Also sets `carry` to the result of `acc == data`.
##### `xori <number>`  
Identical to `xor`, but XORs `acc` with `<number>` in place of `data`.
##### `_xor`  
Identical to `xor`, but does not update the carry bit.
#### AND instructions
##### `and`  
Sets `acc` to a bitwise AND of `acc` and `data`. Also sets `carry` to the result of `acc < data`.
##### `andi <number>`  
Identical to `and`, but ANDs `acc` with `<number>` in place of `data`.
##### `_and`  
Identical to `and`, but does not update the carry bit.
#### OR instructions
##### `or`  
Sets `acc` to a bitwise OR of `acc` and `data`. Also sets `carry` to the result of `acc >= data`.
##### `ori`  
Identical to `or`, but ORs `acc` with `<number>` in place of `data`.
##### `_or`  
Identical to `or`, but does not update the carry bit.
#### Left-shift instructions
##### `sl`  
Shifts the contents of `acc` left by one bit, putting the top bit in `carry`. Fills lowermost bit with 0. This shift most closely matches the behavior of `<<` in most languages.
##### `slc`  
Identical to `sl`, but fills the lowermost bit with the previous value of `carry`.
##### `slr`  
Identical to `sl`, but rotates the uppermost bit into the lowermost bit.
##### `_sl`, `_slc`, `_slr`  
Identical to `sl`, `slc`, and `slr`, respectively, but do not set the carry bit.
#### Right-shift instructions
##### `sr`  
Shifts the contents of `acc` to the left by one bit, putting the bottom bit in `carry`. Fills uppermost bit with 0.
##### `src`  
Identical to `sr`, but fills the uppermost bit with the previous value of `carry`.
##### `srr`  
Identical to `sr`, but rotates the lowermost bit into the uppermost bit.
##### `sra`
Identical to `sr`, but fills the uppermost bit by extending the top bit of `acc`. This shift most closely matches the behavior of `>>` in most languages.
##### `_sr`, `_src`, `_srr`, `_sra`  
Identical to `sr`, `src`, `srr`, and `sra`, respectively, but do not set the carry bit.
