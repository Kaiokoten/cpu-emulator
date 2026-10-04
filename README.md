# CPU Emulator

A CPU emulator in C with an assembler and a step-by-step debugger. Parses a custom assembly
language into an internal instruction list, then executes it on a simple
virtual CPU (16 registers, a flat memory array, flags)

## Build

```bash
cmake -S . -B build
cmake --build build --config Release
```
Or directly with gcc:
```bash
gcc -std=c99 -Wall -Wextra -g src/*.c -o cpu-emulator.exe
```
## Usage

```bash
./cpu-emulator.exe -h                  # run in help mode
./cpu-emulator.exe -r <file.asm>   # run <file.asm> non-interactively (batch mode)   
./cpu-emulator.exe -d <file.asm>   # run <file.asm> in interactive debugger mode
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
Builds the project and runs each `.asm` file in `tests/` in batch mode,
 comparing its output against the matching `.expected` file.

## License

MIT — see [LICENSE](LICENSE).