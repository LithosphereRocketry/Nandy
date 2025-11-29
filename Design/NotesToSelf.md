# Multiple Stacks

A major annoyance of v5.0 is that incrementing the stack pointer is no longer
an atomic operation. This means that interrupts during a stack pointer increment
can potentially cause stack corruption and other nastiness, since they may
see an intermediate non-meaningful state for SP (e.g. while it is swapped for
some unknown value from ACC.) This means that all function prologues and
epilogues, as well as any other situation where the SP is incremented in normal
use, need to be protected by a `dint` ... `eint` guard, wasting 4 cycles and 4
bytes at minimum for every function, probably more if loops also modify the
stack.

One potential solution to this is to assign separate regions of memory to the
interrupt and main-code stacks. 256 bytes of memory is not the cheapest thing,
but compared to the ROM and cycle cost of operation as-is it's not bad.

However, this comes with a problem of its own, specifically related to address
wrap-around. We don't want to maintain separate stack pointers - registers abd
muxes are expensive and initializing the second SP could be an annoyance in
itself. However, this means we're basically just accepting whatever SP the
software hands us and working from it. There's a pitfall here though - offset
addressing. When we say `lds 7`, we're loading from an offset of 7 from the
address 0xFF00 | SP, with our offset applied properly to the full 16-bit
address. A non-obvious consequence of this (at least to me) is that stack reads
and writes may end up landing outside the stack region - a `lds 7` when SP is
0xFE will land at 0xFFFE+7=0x0005 in program ROM. For normal code, we can
trivially avoid this by just making sure SP is initalized correctly and never
asking for offset stack reads that exceed our allocated space. However, for 
interrupt code this is more complicated - we might be handed any SP value, so at
face value we can never use offset memory accesses.

There are a few ways we can work around this. One option is to just accept our
fate and never use offset SP loads in interrupts. Using the _isp macro will
still always wrap around correctly since it only acts on an 8-bit value, so if
we only use the value directly under the stack pointer it will always be in
bounds.

Another option that doesn't require hardware changes is to write some logic that
relocates the stack pointer to a known safe location as part of our interrupt
prologue. This seems tempting, since it lets us use the interrupt stack freely,
but it comes with a _lot_ of potential runtime cost - remember every register
must be saved and restored, so we don't have any scratch space to store values
while we adjust them. It's probably possible to make this work but it adds a lot
of juggling that I'd really like to avoid, especially since interrupts are
really meant to happen fast and get out of the way.

A third option is to simply add the behavior that stack accesses wrap on 256
byte boundaries. This is not that hard to do in the absolute - just a pair of
NAND gates in the carry path - but it is a hardware cost. More annoyingly, it
adds 2 gates (20ns or so) to what is already the critical timing path, making
the processor clock marginally slower. Still probably faster than multiple `isp`
calls for every interrupt prologue, though. Worth thinking about.