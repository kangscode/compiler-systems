# loop-optimization-lab

A small C++ study of **loop transformations and memory hierarchy effects**.

The project compares matrix multiplication variants that a compiler or accelerator compiler might reason about:

- naïve `i-j-k` traversal
- cache-blocked / tiled multiplication
- tiled multiplication with a small inner-loop unroll
- optional compiler optimization reports from Clang

## Why this is compiler-related

Loop transformations are a concrete place where compiler decisions interact with architecture. A transformation can keep the same mathematical result while changing locality, vectorization opportunities, and the amount of data movement.

```text
source loop nest
      │
      ├── interchange / blocking / unrolling
      │
      ▼
access pattern
      │
      ▼
cache / SIMD behavior
      │
      ▼
execution time
```

This is also a useful bridge from compiler IR concepts to AI accelerator work, where tiling and data movement are central optimization decisions.

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Benchmark

```bash
./build/loop_opt_bench --n 512 --tile 32 --repeats 3
```

The program validates all optimized variants against the naïve result before timing them.

## Compiler optimization report

With Clang available:

```bash
./tools/vectorize_report.sh
```

This runs Clang's loop-vectorizer optimization remarks on a small kernel and is meant to connect **source code → optimizer decision → generated code**.

## Notes

- Benchmark results depend strongly on CPU model, cache hierarchy, compiler, frequency scaling, and system load.
- The project deliberately avoids claiming a universal speedup.
- The next step is to inspect generated LLVM IR (`clang++ -S -emit-llvm`) and compare the effect of loop transformations at the IR level.
