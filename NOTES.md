## Overview
0. Intro
1. Project Setup
2. NES Harware - Pt 1 - Cartridges/ROMs
3. Code: ROM reader
4. NES Harware - Pt 1 - CPU (registers)
5. Code: CPU structs
6. NES Harware - Pt 1 - CPU (memory)
7. Code: CPU memory, init and initial run loop
8. NES Harware - Pt 1 - CPU (opcodes)
9. Code: CPU opcodes / instructions
10. "nestest" - verifying our opcodes
11. NES Harware - Pt 2 - PPU
12. Code: PPU (nametables, sprites)
13. Code: "Faking" PPU - Presenting VRAM
14. Extra: Colors?

### 0 - Intro
Let's implement a NES emulator, hardest part of any project is always the name. We are doing a presentation, let's just call it.. NES.. presentation? NES.. press? Ah, Nespresso, that makes sense! Hard part is done, now let's just add some code...

>> "If you're looking for a good first game for your new emulator, try anything made in 1984 or earlier, such as Donkey Kong."

Let's set a goal of this presentation to be able to run Donkey Kong in our emulator. This is our only goal, we wont be able to run every game, we just want that one game to run as a start.

Sounds easy, but where do we start? Setup our project!

### 1 - Project Setup
We are going to implement it in C++, or C++ish, the flavour I'm mostly used to. There isn't any fancy strange syntax, it's essentially C with namespaces and templates. And everyone knows C reads just as pseudocode, right? In this presentation it isn't the language that is important, we just want to see pretty pixels in the end.

I'll try to hide the most boring bits, I'll be using the "genie" project generator tool which will output a makefile for us as long as we just tell it what files to compile.

It's not relevant really, the only thing you need to really care about is that the files we write, we just compile and link it as C++. Use whatever build system you are happy with.

I'll try to split the code up in semi logical components. All the code is based on my old emulator Neeze, I took the core code and tried to throw away anything not nessessary for our goal, there might be stuff that I accidentally left or wont explain in detail. Ask me if something seems odd!

### 2 - NES Hardware - Pt 1
To implement our emulator we first need to understand at least a bit of how the old NES hardware worked.

We will be using mainly NesDev wiki for knowledge gathering.

### 2.1 - NES Roms
Since we wont have an easy way to just plug in an old NES cartrige, we still need to load/read games somehow. Enter "ROMs".

ROMs are files that has some data containing the game code and graphics. Without going into the history here, one of the most popular formats for NES ROMs is "iNES" which thankfully has a pretty straight forward structure.

What is important now, is to know that these files will contain the code and graphics along with some other meta data that we should be able to load in our emulator.

### 2.2 - NES CPU
Once we have the game code from the ROM, how would we be able to execute it? The NES differs quite a lot from modern hardware architecture, we wont be able to just double click and run it (wouldn't be much need for an emulator in that case).

The original NES hardware used a "6502" processer for its CPU, running at 1.79MHz (NTSC version). The "code" we will find in our ROMs are essentially raw instructions that would execute on the 6502 CPU. Since they are different architectures, we need to simulate, well emulate, how the original 6502 CPU would process each instruction.

There are different ways of doing such a thing, but since we are keeping it simple; we are just going to go through them one by one. For each new instruction, we will try to interpet it and simulate each instruction behaviour, as best we can.

However, the instructions wont be able to do much on their own, they need some data to change and interact with. Just like a modern computer, the NES wasn't just a single CPU, to do some actual data processing, the state of the game at runtime needed to be stored somewhere, RAM memory. 2KB to be exact, where 256 bytes were allocated for the stack.

Along with the RAM the CPU also had access to some registers; A, X, Y, PC, S and P.
The first three are the (A)ccumulator, plus the two X and Y index registers, all of them 1 byte (8 bits) each. The accumulator is used mostly for arithmetic logic, figuring out resulting carry and overflow detection. While X and Y are usually used for loop counters.

PC is the Program Counter register, it's 2 bytes, and stores the address of the current location in the game code. So whenever we interpret an instruction, we find the instruction where the PC points to, then we increment the PC to find the next instruction. Since it's 2 bytes we also know that the address space for the NES is 16bits, which will come in handy later.

S is the Stack Pointer, and likewise as the PC points to where we are in the game code, the stack pointer points to the top of the stack.

Finally P is the Status Register, it's essentially 1 byte, ie 8 bits even though not all of them are used. But each bit represents a specific status in the CPU. For example, bit 1 indicates if the last arithmetic operation resulted in a zero or not.

### 2.3 - NES Memory - RAM/bus
Before we jump into code, we will take a quick look at the memory of the NES. As you might recall I said that the PC was 2 bytes wide, ie 16 bits, but the RAM was only 2KB? With 16 bits we could actually address 64KB which might sound strange. So let's look into the memory address space on NesDev:

>> Show image of CPU memory space

We can identify the RAM and stack here, what about "Zero Page"? The first 256 bytes of the address range is actually part of the RAM, but since the first byte is zero, it can be addressed with only one byte. This means instead of having to address the full 16 bit address "0x00??" when reading or writing to RAM, we would only need to address it with "0x??". Since most instructions would have operands, or arguments, telling us at which memory address the instruction would interact with, using Zero Page memory we can effectively save one byte in our game code by only supplying the last part of the address. We will come back to this later on, but for now it's good enough to have seen this.

Above the RAM we see some I/O registers, expansion ROM and SRAM. For our emulator, we wont be implementing either sound, joypad input or save ram, so most of this can be ignored for now. We will however touch apon some IO registers a bit later, but for now we only need to understand that some addresses are connected to other input/output parts of the system.

The PRG ROM, upper and lower banks, is where the CPU would address to access the game code loaded from the ROM file. You might realise now that we need to set our PC, Program Counter, to somewhere in this address space when before running the emulation.

### 3. Code: ROM reader
Enough talk, we should have enough info to start coding something.

Here I have a ROM file that I have legitimately downloaded from a very trustworthy website, I mean dumped myself from a cartrige ofcourse.

Let's have a look at NesDev of the file structure of an iNES ROM.

>> Show file structure

We will skip the part of reading in the file to memory, that isn't interesting. Instead let's implement a function that will parse out the interesting parts of the ROM into a structure, `load_rom_mem()` and `ines_rom_t`.

We want to know how many pages of game code, PRG ROM, and game graphics, CHR ROM the file has. We also need to copy over this data so we can access it later with our CPU. So let's look at some code.

>> Show code..
>> Show ines_rom_t struct...
>> Check that the file is an iNES file
>> Remeber the number of PRG and CHR pages
>> Check if it's a version 1 or 2 iNES format, we will only handle version 1 in this implementation.
>> Parse out what mapper ID the ROM uses.

Short note on mappers, it's too big of a topic for this presentation. But simplified; mappers are extra functionality a game can use, implemented as extra logic in the original cartrige. Such as more PRG or CHR ROM size, on cartrige save, extra "scrolling functionality". We are only interested in mapper zero, which is what Donkey Kong uses.

That's it, we are almost there.

### 4. Code: CPU base
We have the ROM, now let's look at the CPU. Let's setup a structure which holds the registers, CPU memory and pointers that we can fill pointing to our PRG ROM.

>> Show cpu_t struct...

We might also go ahead and add a struct that will act as some kind of encapsulation for our emulator, it might come in handy in the future, but for now it will only store the CPU struct.

>> Show emu_t struct...

Now lets initialize these structs with some data, let's implement `init_emu()`.

>> Show init_emu()

We can check on the NesDev wiki what the registers would be at power up of the original hardware and just fill that in.
We will also allocate memory for the RAM and stack, in my case I opted to have two different arrays for this.

Then we check how many PRG pages the ROM contained, and point our PRG ROM lower and upper to them.

While we are at it, we might as well check NesDev wiki to understand where the Program Counter should start execution from. According to NesDev there are three values at the very end of the PRG ROM which are called "reset vectors". For now we are only interested in the RESET vector, which is the address where execution should start during power up or resetting the console.

---

Now we need to implement our main execution loop, for each update we will "step" the emulation by some unit. Since we are talking about the CPU here, we can call this CPU cycles. Let's look at the implementation.

>> Show main.cpp code
>> Go into step_emu function
>> Go into step_cpu function

We will have a loop where we read from the address that the Program Counter, PC, points to. This will give us the current instruction to execute. We then increment our PC so next iteration we will fetch the next instruction.

We are on good way to executing our PRG, but now we need to implement our simulated instructions, also called opcodes.

### 5. Code: CPU opcodes / instructions
At the location of PC in PRG ROM we will find one byte representing an instruction. And a byte can have 256 different values, this leads us to that potentially we need to implement 256 different instructions... That's a lot. Lets check NesDev for guidence.
https://www.masswerk.at/6502/6502_instruction_set.html
http://www.oxyron.de/html/opcodes02.html
https://www.nesdev.com/6502_cpu.txt
http://users.telenet.be/kim1-6502/6502/proman.html

Our takeaway here is;
1. There is a ton of information about opcodes
2. There is something called addressing modes
3. There are a lot of opcodes, but some seem to do the same logic but using different addressing modes

To be able to finish the presentation, I'll try to do some simplified explanation here and implement one opcode, then I'll leave it as homework for you to actually implement all of them while I bring out a finished list for us to continue.

First let's look at the AND opcode.
>> The AND instructions transfer the accumulator and memory to
     the adder which performs a bit-by-bit AND operation and stores the
     result back in the accumulator.
              This instruction affects the accumulator; Sets the zero flag
     if the result in the accumulator is 0, otherwise resets the zero flag;
     sets the negative flag if the result in the accumulator has bit 7 on,
     otherwise resets the negative flag.

So all instructions that perform an AND does essentially the same thing, they bitwise AND two values. One from the Accumulator register, and one from memory. But how do we know what memory address to perform the operation on? The addressing mode will tell us that.

>> AND is a "Group One" instruction having addressing modes of
     Immediate; Absolute; Zero Page; Absolute,X; Absolute,Y; Zero Page,X;
     Indexed Indirect; and Indirect Indexed.

All these AND instructions do the same logic but using different addressing modes to figure from where in the memory to access the second value. The most easy one to understand is "Absolute":

>> Absolute addressing is a 3-byte instruction.
          The first byte contains the OP CODE for specifying the operation and
     address mode.  The second byte contains the low order byte of the effective
     address (that address which contains the data), while the third byte con-
     tains the high order byte of the effective address.

Compare this to

### --?
But before that, we need to implement a bunch of stuff. As our help we have NesDev wiki and a handly little rom called nestest.


## Simplifications(?)
- Ignore mappers
- Ignore mirroring?
- Ignore scrolling
- Skipping sprite hit-test
- No joycon input
- No sound


