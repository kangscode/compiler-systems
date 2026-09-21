from __future__ import annotations
import math
from dataclasses import dataclass

@dataclass(frozen=True)
class Hardware:
    peak_ops_per_cycle: float = 256.0
    bytes_per_cycle: float = 64.0
    element_bytes: int = 4


def estimate_gemm(M: int, N: int, K: int, tm: int, tn: int, tk: int, hw: Hardware = Hardware()) -> dict[str, float]:
    tiles_m = math.ceil(M / tm); tiles_n = math.ceil(N / tn); tiles_k = math.ceil(K / tk)
    tile_count = tiles_m * tiles_n * tiles_k
    ops = 2 * min(tm, M) * min(tn, N) * min(tk, K) * tile_count
    compute = ops / hw.peak_ops_per_cycle
    bytes_moved = tile_count * (tm * tk + tk * tn + tm * tn) * hw.element_bytes
    memory = bytes_moved / hw.bytes_per_cycle
    return {"tiles": float(tile_count), "compute_cycles": compute, "memory_cycles": memory, "estimated_cycles": max(compute, memory)}
