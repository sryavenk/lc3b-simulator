# Lab 2: LC-3b ISA Simulator

An instruction-level simulator in C for the LC-3b. It runs an assembled program one instruction at a time and updates the registers, condition codes, PC and memory after each one.

## Files

| File | Description |
|------|-------------|
| `lc3bsim2.c` | The simulator (my code is `process_instruction()` at the end) |
| `assembler.linux` | Reference LC-3b assembler provided by the course |

## Building and running

```bash
gcc -std=c99 -o simulate lc3bsim2.c
./assembler.linux program.asm program.obj
./simulate program.obj
```

## Shell commands

| Command | Description |
|---------|-------------|
| `go` | Run until `HALT` |
| `run <n>` | Run `n` instructions |
| `mdump <low> <high>` | Dump memory |
| `rdump` | Dump registers, PC and condition codes |
| `?` | List commands |
| `quit` | Exit |

Dumps are written to a file named `dumpsim`.

## How it works

Each call to `process_instruction()`:

1. **Fetches** the 16-bit word at the PC.
2. **Decodes** the opcode (bits 15–12).
3. **Executes** it, reading from `CURRENT_LATCHES` and writing to `NEXT_LATCHES`.

### Supported instructions

| Group | Instructions |
|-------|--------------|
| Operate | `ADD`, `AND`, `XOR`, `NOT` |
| Shifts | `LSHF`, `RSHFL`, `RSHFA` |
| Branches | `BR` (all 8 variants) |
| Control | `JMP`, `RET`, `JSR`, `JSRR`, `TRAP` |
| Memory | `LDB`, `LDW`, `STB`, `STW`, `LEA` |

### Example

Input:

```asm
        .ORIG x3000
        AND R0, R0, #0
        ADD R0, R0, #5
        LEA R1, D
        LDW R2, R1, #0
        HALT
D       .FILL x1234
        .END
```

`go` then `rdump`:

```
Instruction Count : 5
PC                : 0x0000
CCs: N = 0  Z = 0  P = 1
Registers:
0: 0x0005
1: 0x300a
2: 0x1234
...
7: 0x300a
```

### Limitations

- `RTI` isn't implemented.
- `TRAP` sets the PC to 0, which halts the simulator. Trap routines don't run.
- No memory-mapped I/O.
