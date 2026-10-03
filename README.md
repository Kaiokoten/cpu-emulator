# CPU Emulator

A CPU emulator and assembler written in C with compiler and debugger. Parses a custom assembly
language into an internal instruction list, then executes it on a simple
virtual CPU (16 registers, a flat memory array, flags)

## Build

```bash
gcc -std=c99 -Wall -Wextra -g src/*.c -o asm.exe
```

## Usage

```bash
./asm.exe                    # run program.asm in interactive debugger mode
./asm.exe -c -f <file.asm>   # run <file.asm> non-interactively (batch mode)
./asm.exe -d -f <file.asm>   # run <file.asm> in interactive debugger mode
```

In debugger mode, after each instruction you can inspect:
- `1` — registers
- `2` — flags
- `3` — memory (non-zero cells, as binary)
- `4` — current pc and instruction
- `0` or `\n` / empty line — continue to the next instruction

## Assembly language

Opcodes: `mov`, `add`, `sub`, `dec`, `inc`, `cmp`, `jmp`, `je`, `print`, `halt`.

Operands: a register (`RAX`..`R15`), an immediate value (`5`), a memory
address (`[100]`), or a label.

`jmp` is unconditional; `je` jumps only if the last `cmp` found its operands
equal (zero flag set).

## Tests

```bash
bash tests/run_tests.sh
```
Builds the project and runs each `.asm` file in `tests/` through compiler
mode, comparing its output against the matching `.expected` file.
