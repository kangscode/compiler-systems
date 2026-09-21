from __future__ import annotations
from dataclasses import dataclass, field
from typing import Iterable

@dataclass
class Operation:
    result: str | None
    name: str
    operands: list[str]
    attrs: dict[str, int | float | str] = field(default_factory=dict)

    def uses(self, value: str) -> bool:
        return value in self.operands

    def format(self) -> str:
        lhs = f"{self.result} = " if self.result else ""
        ops = ", ".join(self.operands)
        text = f"  {lhs}{self.name} {ops}".rstrip()
        if self.attrs:
            attrs = ",".join(f"{k}={v}" for k, v in self.attrs.items())
            text += f" {{{attrs}}}"
        return text

@dataclass
class FunctionIR:
    name: str
    ops: list[Operation]

    def format(self) -> str:
        lines = [f"func @{self.name}:"]
        lines.extend(op.format() for op in self.ops)
        return "\n".join(lines)

    def values(self) -> set[str]:
        return {op.result for op in self.ops if op.result}
