# NANDy Programmer's Manual

## About This Guide

## Assembly Syntax Rules
The 


## Common Constructs

## "Hacks" and Advanced Techniques

## Instruction Set
### Program flow instructions
#### Jump instructions
##### `j <label>`  
Jumps to the instruction labeled by <label>. This instruction takes two clock cycles to complete and occupies two bytes in memory.  
##### `jsr <label>`  
Identical to `j`, except the current value of the program counter will be stored to the `data` register. This operation is most commonly used for calling procedures.  
##### `jif`  
Identical to `j`, except the jump will only be taken if the carry bit contains a 1. This will take two cycles to execute regardless of whether the branch is taken.  
##### `ret`  
Jumps to the address stored in `data`, plus one. Replaces the value in `data` with the current value of the program counter as with `jsr`. Unlike the other jump instructions, this instruction only takes a single cycle and single byte of ROM.  

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
#### Halfword manipulation instructions
##### `lu`  
Moves the top 4 bits of `acc` into the bottom 4 bits of `acc`, and replaces the top 4 bits of `acc` with the bottom 4 bits of `data`. Also sets `carry` to the result of `acc != data`. 
##### `lui <number>`  
Moves the top 4 bits of `acc` into the bottom 4 bits of `acc`, and replaces the top 4 bits of `acc` with the bottom 4 bits of `<number>`. Also sets `carry` to the result of `acc != <number>`. Note that two `lui` instructions may be used to load an exact 8-bit value in `acc`.
##### `_lu`  
Identical to `lu`, but does not update the carry bit.
##### `ne`
Sets `carry` to the result of `acc != data`. 
#### XOR instructions
##### `xor`  
Sets `acc` to a bitwise XOR of `acc` and `data`. Also sets `carry` to the result of `acc == data`.
##### `xori <number>`  
Identical to `xor`, but XORs `acc` with `<number>` in place of `data`.
##### `_xor`  
Identical to `xor`, but does not update the carry bit.
##### `eq`  
Sets `carry` to the result of `acc == data`.
#### AND instructions
##### `and`  
##### `andi`  
##### `_and`  
##### `gt`
#### OR instructions
##### `or`  
##### `ori`  
##### `_or`  
##### `le`  
#### Left-shift instructions
##### `sl`  
##### `slc`  
##### `slr`  
##### `sla`  
##### `_sl`, `_slc`, `_slr`, `_sla`  
##### `top`
#### Right-shift instructions
##### `sr`  
##### `src`  
##### `srr`  
##### `sra`  
##### `_sr`, `_src`, `_srr`, `_sra`  
##### `bot`
