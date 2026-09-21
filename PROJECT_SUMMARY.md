# Project summary for an internship email/CV

## 1. tinyc-llvm
**One-line CV bullet:**
- C++20로 tiny C-like language의 lexer/parser/AST를 구현하고 SSA-style temporary를 사용하는 LLVM IR code generator 제작.

**What to demonstrate in an interview:**
- why AST is separate from code generation
- operator precedence in recursive-descent parsing
- how an expression becomes SSA values
- why LLVM IR is useful as a target-independent representation

## 2. mlir-npu-optimizer
**One-line CV bullet:**
- MLIR의 IR/pass/lowering 개념을 학습하기 위해 tensor IR을 직접 설계하고 constant folding, operator fusion, tiling, NPU lowering 및 간단한 cost model 구현.

**Important wording:**
- "MLIR-inspired" / "MLIR 개념 학습용" is accurate.
- Do not imply that this project uses the official MLIR framework.

**What to demonstrate in an interview:**
- why fusion can reduce intermediate memory traffic
- why tiling is tied to local memory/cache behavior
- why target-specific lowering should happen after higher-level transformations
- what a cost model can and cannot predict

## 3. loop-optimization-lab
**One-line CV bullet:**
- C++ 행렬곱을 대상으로 naive/tiled/unrolled loop variants를 비교하고 correctness, runtime, Clang vectorization remarks를 함께 측정.

**What to demonstrate in an interview:**
- why loop order changes locality
- what blocking/tiling changes in memory behavior
- why manual unrolling is not automatically faster
- how compiler optimization remarks help validate hypotheses

## Suggested GitHub descriptions

**tinyc-llvm**
> Tiny C-like compiler in C++20: recursive-descent parser and SSA-style LLVM IR generation.

**mlir-npu-optimizer**
> MLIR-inspired tensor IR playground for studying operator fusion, tiling, lowering, and accelerator cost modeling.

**loop-optimization-lab**
> Experimental study of loop transformations, cache locality, and compiler vectorization using C++ matrix multiplication.
