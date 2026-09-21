# Compiler / Systems Portfolio

Three small, end-to-end self-study projects prepared as a GitHub portfolio for a computer-systems / AI-accelerator research internship application.

## Portfolio map

| Project | Main evidence | Stack |
|---|---|---|
| `tinyc-llvm` | front-end → AST → SSA-style LLVM IR | C++20, CMake, Clang |
| `mlir-npu-optimizer` | IR + passes + fusion + tiling + lowering + cost model | Python 3 |
| `loop-optimization-lab` | loop transformation + cache locality + vectorization | C++20, CMake, Clang |

## Recommended order on GitHub

1. **tinyc-llvm** — shows that the compiler pipeline is understood from the front end down to LLVM IR.
2. **mlir-npu-optimizer** — shows interest in accelerator compilation and HW/SW co-design concepts.
3. **loop-optimization-lab** — shows that optimization decisions are connected to memory hierarchy and generated-code behavior.

## What to say about these projects

Use conservative language:

> "컴파일러 기초를 독학하며 작은 컴파일러와 MLIR-inspired NPU IR optimization playground를 구현해 보았습니다. 특히 LLVM IR 생성, IR transformation, operator fusion, tiling, lowering, loop optimization을 직접 다뤄보면서 컴파일러와 하드웨어의 경계에 관심을 갖게 되었습니다."

Do **not** describe `mlir-npu-optimizer` as an implementation of MLIR. It is explicitly a small MLIR-inspired teaching project.
