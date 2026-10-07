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
./cpu-emulator.exe -h              # show help
./cpu-emulator.exe -r <file.asm>   # run <file.asm> non-interactively (batch mode)
./cpu-emulator.exe -d <file.asm>   # run <file.asm> in interactive debugger mode
```

The exit code is `0` on success and `1` if the program could not be
assembled or stopped with a runtime error.

In debugger mode, after each instruction you can inspect:
- `1` — registers
- `2` — flags
- `3` — memory (non-zero cells, as binary)
- `4` — current pc and instruction
- `0` or `\n` / empty line — continue to the next instruction

## Assembly language

### Example

```asm
# Count down from 3 and print each value
        mov RCX, 3
loop:   print RCX
        dec RCX
        cmp RCX, 0
        je done
        jmp loop
done:   halt
```

Output:

```
3
2
1
```

### Syntax

One instruction per line:

```
[label:] opcode [operand1[, operand2]]   # comment
```

- **Comments** start with `#` and run to the end of the line.
- **Operands** are separated by commas and/or whitespace (spaces or tabs):
  `mov RAX, 5`, `mov RAX,5` and `mov RAX 5` are the same.
- **Case:** opcodes and register names are case-insensitive (`MOV rax, 5`
  is valid); labels are case-sensitive (`Loop` and `loop` are different labels).
- **Labels** are declared as `name:`, either on their own line or before an
  instruction on the same line (`start: inc RAX`). A jump may refer to a label
  declared later in the file.
- **Label names** start with a letter or `_`, continue with letters, digits
  or `_` (`loop_2`, `_start`), are at most 49 characters long, must not
  repeat a register or an opcode name (in any case), and must be unique.
- Empty lines are ignored; both LF and CRLF line endings are accepted.
- A line can be at most 149 characters long.

### Instructions

Operand kinds: **r** — register, **m** — memory cell, **i** — immediate,
**l** — label.

| Instruction | Operands | Effect |
|---|---|---|
| `mov dst, src` | `dst`: r, m; `src`: r, m, i | `dst = src` |
| `add dst, src` | `dst`: r, m; `src`: r, m, i | `dst = dst + src` |
| `sub dst, src` | `dst`: r, m; `src`: r, m, i | `dst = dst - src` |
| `inc dst` | `dst`: r, m | `dst = dst + 1` |
| `dec dst` | `dst`: r, m | `dst = dst - 1` |
| `cmp a, b` | `a`: r, m, i; `b`: r, m, i | sets the zero flag `ZF` to 1 if `a == b`, otherwise to 0 |
| `jmp label` | l | jumps to `label` |
| `je label` | l | jumps to `label` if `ZF` is 1 |
| `print src` | r, m, i | prints the value of `src` followed by a newline |
| `halt` | — | stops the program |

The assembler checks both the number of operands and their kinds, so an
instruction such as `mov 5, RAX` or `jmp RAX` is rejected before the program
runs.

### Operands

| Kind | Syntax |
|---|---|
| Register | `RAX`, `RBX`, `RCX`, `RDX`, `RSI`, `RDI`, `RBP`, `RSP`, `R8`–`R15` |
| Memory cell | `[N]`, where `N` is a decimal number from `0` to `4095` |
| Immediate | a decimal integer with an optional sign, e.g. `42`, `-7`, `+5` |
| Label | a name declared with `name:` |

Registers and memory cells hold signed 64-bit integers, so immediates range
from `-9223372036854775808` to `9223372036854775807`. Only decimal notation is
supported. A word that starts with a digit or a sign is always parsed as a
number.

### Errors

Assembly errors are reported with a line number and stop the program before
it runs, for example:

```
line 1: Syntax error: movx opcode was not found.
line 1: memory address out of range '[5000]'.
line 2: 'MOV' requires 2 operands, got 1.
line 3: 'JMP' operand 1 has a wrong type.
line 3: label 'a' mentioned multiple times.
line 5: Syntax error: nowhere label was not found.
```

### Known limitations

- There is no stack, no `call`/`ret`, and `ZF` is the only flag.
- Memory addresses must be constants: indirect addressing such as `[RAX]`
  is not supported.

## Tests

```bash
bash tests/run_tests.sh
```

Builds the project and runs each `.asm` file in `tests/` in batch mode,
comparing its output against the matching `.expected` file.

On Windows, run it from Git Bash (in PowerShell, `bash` starts WSL instead):

```powershell
& "C:\Program Files\Git\bin\bash.exe" tests/run_tests.sh
```

## License

MIT — see [LICENSE](LICENSE).