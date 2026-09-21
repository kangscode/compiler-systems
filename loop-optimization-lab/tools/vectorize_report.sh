#!/usr/bin/env bash
set -euo pipefail
TMP_DIR="$(mktemp -d)"
trap 'rm -rf "$TMP_DIR"' EXIT
cat > "$TMP_DIR/kernel.cpp" <<'CPP'
void saxpy(float* __restrict y, const float* __restrict x, float a, int n) {
    for (int i = 0; i < n; ++i) y[i] = a * x[i] + y[i];
}
CPP
clang++ -O3 -Rpass=loop-vectorize -Rpass-missed=loop-vectorize -c "$TMP_DIR/kernel.cpp" -o "$TMP_DIR/kernel.o" 2>&1 || true
