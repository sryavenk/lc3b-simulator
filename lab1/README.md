# Lab 1: LC-3b Assembler

A two-pass assembler in C that converts LC-3b assembly into machine code for the later labs' simulators.

## Files

| File | Description |
|------|-------------|
| `assembler.c` | The assembler |
| `mul_bytes.asm` | LC-3b program that multiplies two bytes |

## Building and running

```bash
gcc -std=c99 -o assemble assembler.c
./assemble <source.asm> <output.obj>
```

## Supported syntax

```
[label] opcode operands ; comment
```

- Case-insensitive
- Comments start with `;`
- Constants: `#` for decimal (`#-3`), `x` for hex (`x1F`)

### Instructions

| Group | Instructions |
|-------|--------------|
| Operate | `ADD`, `AND`, `XOR`, `NOT` |
| Shifts | `LSHF`, `RSHFL`, `RSHFA` |
| Branches | `BR` (all 8 variants) |
| Control | `JMP`, `RET`, `JSR`, `JSRR`, `RTI`, `TRAP` |
| Memory | `LDB`, `LDW`, `STB`, `STW`, `LEA` |
| Special | `HALT`, `NOP` |

### Pseudo-ops

| Pseudo-op | Description |
|-----------|-------------|
| `.ORIG N` | Start address |
| `.FILL W` | Stores a 16-bit value |
| `.END` | End of program |

## How it works

1. **Pass 1:** builds a symbol table of label addresses.
2. **Pass 2:** encodes each instruction as a hex word.

### Example

Input:

```asm
        .ORIG #4096
A       LEA R1, Y
        LDW R1, R1, #0
        LDW R1, R1, #0
        ADD R1, R1, R1
        ADD R1, R1, x-10
        BRN A
        HALT
Y       .FILL #263
        .FILL #13
        .FILL #6
        .END
```

Output:

```
0x1000
0xE206
0x6240
0x6240
0x1241
0x1270
0x09FA
0xF025
0x0107
0x000D
0x0006
```

## Exit codes

| Code | Meaning |
|------|---------|
| 0 | Success |
| 1 | Wrong number of arguments |
| 2 | No `.ORIG` found |
| 4 | File can't be opened, or invalid constant |

### Limitations

These aren't reported as errors:

- Undefined labels
- Unknown opcodes
- Out-of-range constants and offsets
- Duplicate or invalid labels
- Bad registers or wrong operand count

## `mul_bytes.asm`

Multiplies two unsigned bytes by repeated addition, then stores the product's low byte and an overflow flag.
