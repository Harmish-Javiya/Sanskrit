# Sans — A Sanskrit-Inspired Programming Language

Sans is a compiled programming language with Sanskrit-inspired English keywords, targeting x86_64 macOS assembly. It features a complete compiler pipeline including lexer, parser, AST, semantic analysis, and native code generation.

---

## Philosophy

Sans is designed to feel familiar to C++ developers while incorporating unique Sanskrit-inspired syntax differences:

- **Type after variable name** — `x purnank = 10;` instead of `int x = 10;`
- **Verb at end for print** — `x mudraya;` instead of `print(x);`
- **Word operators in conditions** — `yadi x adhik 5` instead of `if (x > 5)`
- **Natural for loop** — `krute i : 0 se 10` instead of `for(int i = 0; i < 10; i++)`

---

## Keyword Reference

### Data Types
| Sans | Meaning |
|------|---------|
| `purnank` | int |
| `dashamlav` | float |
| `aksarmala` | string |
| `aksar` | char |
| `tark` | bool |
| `sarani` | array |
| `sunya` | void/null |
| `sthir` | const |

### Control Flow
| Sans | Meaning |
|------|---------|
| `yadi` | if |
| `anyatha` | else |
| `yavat` | while |
| `krute` | for |
| `se` | from (used in range loop) |
| `viram` | break |
| `anuvarta` | continue |
| `vikalp` | switch |
| `prakaran` | case |
| `adya` | default |

### Functions & Program
| Sans | Meaning |
|------|---------|
| `karma` | function |
| `mukhya` | main |
| `nivarta` | return |
| `nirgachh` | exit |

### I/O
| Sans | Meaning |
|------|---------|
| `mudraya` | print |
| `grahan` | input |

### Boolean Literals
| Sans | Meaning |
|------|---------|
| `satya` | true |
| `asatya` | false |

### Memory
| Sans | Meaning |
|------|---------|
| `nava` | new (allocate) |
| `mukta` | free (deallocate) |

### Word-Based Comparison Operators
| Sans | Meaning |
|------|---------|
| `adhik` | > |
| `nyun` | < |
| `sam` | == |
| `asam` | != |
| `adhik_sam` | >= |
| `nyun_sam` | <= |

---

## Syntax Examples

### Variable Declaration
```
x purnank = 10;
pi dashamlav = 3.14;
naam aksarmala = "Arjun";
ch aksar = 'A';
flag tark = satya;
MAX sthir purnank = 100;
arr sarani purnank = [1, 2, 3];
```

### Print & Input
```
x mudraya;
"namaste" mudraya;
x grahan;
```

### If / Else
```
yadi x adhik 5 {
    "bada hai" mudraya;
} anyatha {
    "chota hai" mudraya;
}
```

### While Loop
```
yavat x adhik 0 {
    x -= 1;
}
```

### For Loop — Range Style
```
krute i : 0 se 10 {
    i mudraya;
}
```

### For Loop — Native Style
```
krute (i purnank = 0; i nyun 10; i++) {
    i mudraya;
}
```

### Switch / Case
```
vikalp x {
    prakaran 1: "ek" mudraya; viram;
    prakaran 2: "do" mudraya; viram;
    adya: "aur" mudraya;
}
```

### Functions
```
karma jodan(a purnank, b purnank) purnank {
    nivarta a + b;
}

mukhya() {
    result purnank = jodan(5, 10);
    result mudraya;
    nivarta;
}
```

### Arrays
```
arr sarani purnank = [1, 2, 3];
arr[0] mudraya;
arr[1] = 99;
```

### Memory
```
ptr purnank = nava purnank;
mukta ptr;
```

### Exit
```
nirgachh(0);
```

---

## Compiler Pipeline

```
Source (.sans)
      ↓
   Lexer          → tokenizes source into token stream
      ↓
   Parser         → builds Abstract Syntax Tree (AST)
      ↓
   Semantic       → type checking, scope checking
   Analysis
      ↓
   Code           → emits x86_64 macOS assembly
   Generator
      ↓
   nasm           → assembles to object file
      ↓
   ld             → links to binary
      ↓
   Binary         → runs natively
```

---

## Project Structure

```
src/
├── includes/
│   ├── lexer.hpp        → TokenType enum, Token struct
│   ├── ast.hpp          → all AST node structs
│   ├── parser.hpp       → Parser class declaration
│   ├── sementic.hpp     → SemanticAnalyzer, SymbolTable
│   └── codegen.hpp      → CodeGenerator class
├── lexer.cpp            → tokenize() implementation
├── parser.cpp           → all parse functions
├── sementic.cpp         → all visit functions
├── codegen.cpp          → all code generation functions
├── main.cpp             → compiler entry point
├── Makefile             → build system
└── test.sans            → test program
```

---

## Building

### Requirements
- g++ with C++17 support
- nasm
- macOS (x86_64)

### Install nasm
```bash
brew install nasm
```

### Build Compiler
```bash
make build
```

### Compile a Sans Program
```bash
./compiler program.sans
nasm -f macho64 output.asm -o output.o
ld -o program output.o -lSystem -syslibroot `xcrun -sdk macosx --show-sdk-path` -e _main -arch x86_64
./program
```

---

## Semantic Analysis

The semantic analyzer performs:

- **Scope checking** — variables must be declared before use
- **Type checking** — types must be compatible in expressions
- **Const checking** — const variables cannot be reassigned
- **Function checking** — correct argument count and types
- **Return type checking** — return value must match function type

### Symbol Table

Uses a scoped symbol table implemented as a vector of hash maps:

```
global scope  → functions, global variables
  function scope → parameters
    block scope  → local variables
      nested block → more local variables
```

Lookup searches from innermost scope outward — local variables shadow outer ones.

---

## Code Generation

Targets x86_64 macOS assembly using nasm.

### Key Design Decisions

- All expressions leave their result in `rax`
- Local variables stored on stack relative to `rbp`
- Follows System V AMD64 calling convention
- String literals stored in `.data` section
- Mac syscalls used for I/O and exit

### Stack Layout
```
rbp       → saved base pointer
rbp-8     → first local variable
rbp-16    → second local variable
rbp-24    → third local variable
...
rsp       → current stack top
```

### Calling Convention
```
arg1 → rdi
arg2 → rsi
arg3 → rdx
arg4 → rcx
arg5 → r8
arg6 → r9
return value → rax
```

---

## Future Plans

- Linux x86_64 support
- Windows x86_64 support
- Standard library
- Global variables with `vishwa` keyword
- Bytecode VM backend
- LLVM backend
- Devanagari script support

---

Built as a systems programming learning project combining compiler design, assembly programming, and Sanskrit language inspiration.

have fun