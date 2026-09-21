from __future__ import annotations
import re
from .ir import FunctionIR, Operation

_ATTR = re.compile(r"\{(.*)\}")

def _parse_attrs(text: str) -> dict[str, int | float | str]:
    if not text:
        return {}
    out: dict[str, int | float | str] = {}
    for item in text.split(","):
        if not item.strip():
            continue
        k, v = item.split("=", 1)
        v = v.strip()
        try:
            out[k.strip()] = int(v)
        except ValueError:
            try:
                out[k.strip()] = float(v)
            except ValueError:
                out[k.strip()] = v
    return out

def parse(text: str) -> FunctionIR:
    lines = [line.strip() for line in text.splitlines() if line.strip() and not line.strip().startswith("#")]
    if not lines or not lines[0].startswith("func @") or not lines[0].endswith(":"):
        raise ValueError("expected 'func @name:' header")
    name = lines[0][6:-1]
    ops: list[Operation] = []
    for line in lines[1:]:
        result = None
        if line.startswith("return "):
            ops.append(Operation(None, "return", [line[len("return "):].strip()]))
            continue
        if " = " in line:
            result, rhs = line.split(" = ", 1)
        else:
            rhs = line
        attr_match = _ATTR.search(rhs)
        attrs = _parse_attrs(attr_match.group(1)) if attr_match else {}
        if attr_match:
            rhs = rhs[:attr_match.start()].strip()
        parts = rhs.split(None, 1)
        if len(parts) == 1:
            opname, operands_text = parts[0], ""
        else:
            opname, operands_text = parts
        operands = [x.strip() for x in operands_text.split(",") if x.strip()]
        ops.append(Operation(result, opname, operands, attrs))
    return FunctionIR(name, ops)
