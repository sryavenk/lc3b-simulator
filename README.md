# LC-3b Computer Architecture Labs

Labs from EE 460N (Computer Architecture) at UT Austin. Together they build up a simulator for the LC-3b, a 16-bit educational ISA. It starts as an assembler and ends as a pipelined processor with interrupts, exceptions and virtual memory. Each lab extends the one before it.

## Labs

| Lab | Topic | What I built |
|-----|-------|--------------|
| 1 | Assembler | Two-pass assembler that turns LC-3b assembly into machine code |
| 2 | ISA simulator | Simulator that runs one instruction per step |
| 3 | Microarchitecture | Cycle-level simulator driven by a microcoded control store |
| 4 | Interrupts and exceptions | Timer interrupt, three exceptions and `RTI`, added to the Lab 3 design |
| 5 | Virtual memory | Address translation, page faults and PTE-based protection |
| 6 | Pipelining | 5-stage pipeline with dependency and control stalls |

Each lab folder has its own README with diagrams, build steps and an example run.

## Requirements

- `gcc` with C99 support
- Linux, to run the provided `assembler.linux` binary

## Quick start

From any lab folder:

```bash
gcc -std=c99 -o simulate lc3bsimN.c       # N = 2–6
./assembler.linux program.asm program.obj
./simulate [ucodeN] program.obj
```

Labs 3–6 take a microcode file as the first argument, and Lab 5 also takes a page table file. Lab 1 builds the assembler itself:

```bash
gcc -std=c99 -o assemble assembler.c
./assemble program.asm program.obj
```
