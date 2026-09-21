# tinyc-llvm

A small C++20 compiler that lowers a tiny C-like language to textual LLVM IR.

> **Purpose:** self-study project for understanding the compiler pipeline: lexing → parsing → AST → semantic checks → SSA-style code generation → LLVM IR.

This is intentionally small enough to read end-to-end. It is not intended to be a production compiler.

## Supported language

```text
fn main() {
  let x = 10 + 20 * 2;
  let y = x - 5;
  return y;
}
```

Supported constructs:

- integer literals
- `let` bindings
- `+ - * /`
- parentheses
- `return`
- a single `main` function

## Pipeline

```text
source.tiny
   │
   ▼
Lexer ── tokens ──▶ Parser ── AST ──▶ LLVM IR emitter
                                      │
                                      ▼
                                   main.ll
                                      │
                                      ▼
                               clang / LLVM tools
```

The emitter uses SSA-style temporary values rather than a mutable local-variable representation. This makes the connection between an expression tree and LLVM IR explicit.

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/tinyc examples/arith.tiny -o /tmp/arith.ll
cat /tmp/arith.ll

clang /tmp/arith.ll -o /tmp/arith
/tmp/arith
echo $?
```

Expected exit status: `45`.

## Tests

```bash
ctest --test-dir build --output-on-failure
```

## What I learned

- how recursive-descent parsing maps grammar rules to functions
- why an AST is useful as an intermediate representation before lowering
- how arithmetic expressions become SSA values
- how a compiler can emit LLVM IR without directly constructing machine instructions
- why target-specific code generation is separated from the front end

## Next extensions

- comparisons and conditional branches
- explicit basic blocks / CFG construction
- LLVM `phi` nodes
- a real optimization pass
- replacing textual LLVM IR emission with LLVM's C++ IRBuilder API
