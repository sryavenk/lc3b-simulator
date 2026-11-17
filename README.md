# LC-3b Computer Architecture Labs

Labs from EE 460N (Computer Architecture) at UT Austin. Together they build up a simulator for the LC-3b, a 16-bit educational ISA. It starts as an assembler and ends as a pipelined processor with interrupts, exceptions and virtual memory. Each lab extends the one before it.

## Labs

| Lab | Topic | What I built |
|-----|-------|--------------|
| [Lab 1](lab1) | Assembler | Two-pass assembler that turns LC-3b assembly into machine code |
| [Lab 2](lab2) | ISA simulator | Simulator that runs one instruction per step |
| [Lab 3](lab3) | Microarchitecture | Cycle-level simulator driven by a microcoded control store |
| [Lab 4](lab4) | Interrupts and exceptions | Timer interrupt, three exceptions and `RTI`, added to the Lab 3 design |
| [Lab 5](lab5) | Virtual memory | Address translation, page faults and PTE-based protection |
| [Lab 6](lab6) | Pipelining | 5-stage pipeline with dependency and control stalls |

Each lab folder has its own README with diagrams, build steps and an example run.
