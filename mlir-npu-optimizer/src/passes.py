from __future__ import annotations
from .ir import FunctionIR, Operation


def constant_fold(func: FunctionIR) -> int:
    constants: dict[str, float] = {}
    changed = 0
    for op in func.ops:
        if op.name == "const" and op.result and op.operands:
            constants[op.result] = float(op.operands[0])
        elif op.name in {"add", "mul", "sub"} and op.result and len(op.operands) == 2:
            a = constants.get(op.operands[0]); b = constants.get(op.operands[1])
            if a is not None and b is not None:
                if op.name == "add": v = a + b
                elif op.name == "mul": v = a * b
                else: v = a - b
                op.name = "const"; op.operands = [str(int(v) if v.is_integer() else v)]; op.attrs = {}
                constants[op.result] = v; changed += 1
    return changed


def fuse_mma_bias_relu(func: FunctionIR) -> int:
    out: list[Operation] = []
    i = 0; changed = 0
    while i < len(func.ops):
        if i + 2 < len(func.ops):
            a, b, c = func.ops[i:i+3]
            if a.name == "npu.matmul" and a.result and b.name == "npu.add" and b.result and c.name == "npu.relu" and c.result:
                if b.operands and b.operands[0] == a.result and c.operands == [b.result]:
                    fused = Operation(c.result, "npu.fused_mma_bias_relu", [*a.operands, b.operands[1]], dict(a.attrs))
                    out.append(fused); i += 3; changed += 1; continue
        out.append(func.ops[i]); i += 1
    func.ops = out
    return changed


def tile_matmul(func: FunctionIR, tm: int, tn: int, tk: int) -> int:
    changed = 0
    for op in func.ops:
        if op.name in {"npu.matmul", "npu.fused_mma_bias_relu"} and {"M", "N", "K"}.issubset(op.attrs):
            op.attrs.update({"TM": tm, "TN": tn, "TK": tk})
            op.attrs["tiles_M"] = (int(op.attrs["M"]) + tm - 1) // tm
            op.attrs["tiles_N"] = (int(op.attrs["N"]) + tn - 1) // tn
            op.attrs["tiles_K"] = (int(op.attrs["K"]) + tk - 1) // tk
            changed += 1
    return changed


def lower_to_npu(func: FunctionIR) -> int:
    lowered = 0
    for op in func.ops:
        if op.name == "npu.matmul": op.name = "npu.launch_gemm"
        elif op.name == "npu.fused_mma_bias_relu": op.name = "npu.launch_fused_gemm_bias_relu"
        elif op.name == "npu.add": op.name = "npu.vector_add"
        elif op.name == "npu.relu": op.name = "npu.vector_relu"
        if op.name.startswith("npu."):
            lowered += 1
    return lowered
