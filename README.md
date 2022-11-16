# NANDy
Nandy (capitalization variable) is an 8-bit RISC computer architecture built for implementation using simple 74xx-series discrete NAND gates. It was inspired by the theorem shown in most digital logic classes that any piece of combinational logic may be recreated as a network of NAND gates.
## Design Principles
The NANDy architecture was built around several design principles:
1. Simplicity. I have not a lot of budget for this project and even less time to spend soldering hundreds of components. Therefore, the primary goal is to make this CPU as simple as possible.
2. Usability. While I could make a [Subleq](https://en.wikipedia.org/wiki/One-instruction_set_computer#Subtract_and_branch_if_less_than_or_equal_to_zero "Wikipedia: One-instruction set computer: Subleq") or [Brainfuck](https://en.wikipedia.org/wiki/Brainfuck "Wikipedia: Brainfuck") machine with potentially less components, these architectures are simplistic to the point of being painful to use for even basic tasks. Therefore, the ISA of this processor must be capable enough that someone might actually want to use it.
3. Performance. This is the least important design tenet to me; this is primarily a demonstration and not really designed to accomplish any particular task. However, it would be pretty neat if it could run at a respectable clockspeed.
## Architecture
NANDy uses a simple Harvard accumulator architecture, with one accumulator register, one data register, and one stack pointer register. All mathematical operations place their result in the accumulator with the exception of stack-pointer increment instructions. Data memory and program memory each provide 256 bytes of total storage in separate address spaces.
The architecture is implemented as a single cycle, with all register reads continuous and all register writes on the same clock edge.
