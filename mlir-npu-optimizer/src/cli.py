from __future__ import annotations
import argparse
from pathlib import Path
from .parser import parse
from .passes import constant_fold, fuse_mma_bias_relu, tile_matmul, lower_to_npu
from .cost_model import estimate_gemm

def main() -> int:
    p = argparse.ArgumentParser(description="MLIR-inspired NPU IR optimizer")
    p.add_argument("input", type=Path)
    p.add_argument("--fuse", action="store_true")
    p.add_argument("--tile", nargs=3, type=int, metavar=("TM", "TN", "TK"))
    p.add_argument("--lower", action="store_true")
    p.add_argument("--cost", action="store_true")
    args = p.parse_args()
    func = parse(args.input.read_text())
    print("=== input ===")
    print(func.format())
    print()
    print(f"constant_fold: {constant_fold(func)}")
    if args.fuse: print(f"fusion: {fuse_mma_bias_relu(func)}")
    if args.tile: print(f"tiling: {tile_matmul(func, *args.tile)}")
    if args.lower: print(f"lowering: {lower_to_npu(func)}")
    print("\n=== output ===")
    print(func.format())
    if args.cost:
        for op in func.ops:
            if op.name.startswith("npu.launch") and {"M", "N", "K", "TM", "TN", "TK"}.issubset(op.attrs):
                e = estimate_gemm(int(op.attrs["M"]), int(op.attrs["N"]), int(op.attrs["K"]), int(op.attrs["TM"]), int(op.attrs["TN"]), int(op.attrs["TK"]))
                print("\n=== estimated cost ===")
                for k, v in e.items(): print(f"{k}: {v:.2f}")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
