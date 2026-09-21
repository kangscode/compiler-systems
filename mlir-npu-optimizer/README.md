# mlir-npu-optimizer

A **small MLIR-inspired accelerator compiler playground** written in Python.

The goal is not to reimplement MLIR. Instead, it isolates the compiler ideas that matter in an accelerator stack:

```text
High-level tensor IR
        │
        ├── canonicalization / constant folding
        ├── operator fusion
        ├── tiling + launch configuration
        └── lowering to target-like NPU ops
        │
        ▼
Estimated execution cost
```

This project is intentionally explicit about being **MLIR-inspired rather than actual MLIR**. It is a self-study project for understanding IR, transformation passes, lowering, and cost models before using the real MLIR framework.

## Example

Input:

```text
func @main:
  %m = npu.matmul %A, %B {M=256,N=256,K=128}
  %a = npu.add %m, %bias
  %r = npu.relu %a
  return %r
```

After fusion and lowering:

```text
func @main:
  %f = npu.fused_mma_bias_relu %A, %B, %bias {M=256,N=256,K=128,TM=32,TN=32,TK=32}
  return %f
```

The cost model compares tile sizes using a simple compute-vs-memory model.

## Run

```bash
python -m src.cli examples/gemm.ir --fuse --tile 32 32 32 --lower --cost
```

## Tests

```bash
python -m unittest discover -s tests -v
```

## Project structure

```text
src/
  ir.py          IR data model
  parser.py      textual IR parser/printer
  passes.py      compiler passes
  cost_model.py  tile-level cost model
  cli.py         command-line driver
examples/
  gemm.ir
  conv_like.ir
tests/
  test_pipeline.py
```

## Why I built this

The project focuses on the layer between a tensor program and an accelerator. The interesting compiler questions are:

1. Which operations can be fused?
2. How does tile size change data movement?
3. What information must survive lowering?
4. How should a compiler expose target-specific constraints without changing the source program?

## Honest scope

- No external MLIR dependency.
- No claims of implementing production MLIR infrastructure.
- The cost model is analytical and educational, not a hardware simulator.
- The target ops are pseudo-NPU operations designed to make lowering steps inspectable.

A natural next step is to port the same pass structure to a real MLIR dialect and connect it to LLVM lowering.
