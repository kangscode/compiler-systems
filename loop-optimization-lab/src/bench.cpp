#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

struct Matrix {
    std::size_t n;
    std::vector<float> x;
    explicit Matrix(std::size_t n_) : n(n_), x(n_ * n_, 0.0f) {}
    float& at(std::size_t i, std::size_t j) { return x[i * n + j]; }
    const float& at(std::size_t i, std::size_t j) const { return x[i * n + j]; }
};

void matmul_naive(const Matrix& A, const Matrix& B, Matrix& C) {
    const std::size_t n = A.n;
    std::fill(C.x.begin(), C.x.end(), 0.0f);
    for (std::size_t i = 0; i < n; ++i)
        for (std::size_t j = 0; j < n; ++j)
            for (std::size_t k = 0; k < n; ++k)
                C.at(i, j) += A.at(i, k) * B.at(k, j);
}

void matmul_tiled(const Matrix& A, const Matrix& B, Matrix& C, std::size_t T) {
    const std::size_t n = A.n;
    std::fill(C.x.begin(), C.x.end(), 0.0f);
    for (std::size_t ii = 0; ii < n; ii += T)
        for (std::size_t kk = 0; kk < n; kk += T)
            for (std::size_t jj = 0; jj < n; jj += T)
                for (std::size_t i = ii; i < std::min(ii + T, n); ++i)
                    for (std::size_t k = kk; k < std::min(kk + T, n); ++k) {
                        const float a = A.at(i, k);
                        for (std::size_t j = jj; j < std::min(jj + T, n); ++j)
                            C.at(i, j) += a * B.at(k, j);
                    }
}

void matmul_tiled_unrolled(const Matrix& A, const Matrix& B, Matrix& C, std::size_t T) {
    const std::size_t n = A.n;
    std::fill(C.x.begin(), C.x.end(), 0.0f);
    for (std::size_t ii = 0; ii < n; ii += T)
        for (std::size_t kk = 0; kk < n; kk += T)
            for (std::size_t jj = 0; jj < n; jj += T)
                for (std::size_t i = ii; i < std::min(ii + T, n); ++i)
                    for (std::size_t k = kk; k < std::min(kk + T, n); ++k) {
                        const float a = A.at(i, k);
                        const std::size_t end = std::min(jj + T, n);
                        std::size_t j = jj;
                        for (; j + 3 < end; j += 4) {
                            C.at(i, j) += a * B.at(k, j);
                            C.at(i, j + 1) += a * B.at(k, j + 1);
                            C.at(i, j + 2) += a * B.at(k, j + 2);
                            C.at(i, j + 3) += a * B.at(k, j + 3);
                        }
                        for (; j < end; ++j) C.at(i, j) += a * B.at(k, j);
                    }
}

float max_abs_diff(const Matrix& A, const Matrix& B) {
    float m = 0.0f;
    for (std::size_t i = 0; i < A.x.size(); ++i) m = std::max(m, std::abs(A.x[i] - B.x[i]));
    return m;
}

template <typename F>
double seconds(F&& fn, int repeats) {
    double total = 0.0;
    for (int r = 0; r < repeats; ++r) {
        const auto start = std::chrono::steady_clock::now();
        fn();
        const auto end = std::chrono::steady_clock::now();
        total += std::chrono::duration<double>(end - start).count();
    }
    return total / repeats;
}

int main(int argc, char** argv) {
    std::size_t n = 256, tile = 32; int repeats = 3; bool check_only = false;
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        auto next = [&](const char* flag) -> std::string {
            if (i + 1 >= argc) throw std::runtime_error(std::string("missing value for ") + flag);
            return argv[++i];
        };
        if (a == "--n") n = std::stoull(next("--n"));
        else if (a == "--tile") tile = std::stoull(next("--tile"));
        else if (a == "--repeats") repeats = std::stoi(next("--repeats"));
        else if (a == "--check-only") check_only = true;
        else throw std::runtime_error("unknown option: " + a);
    }
    if (n == 0 || tile == 0 || repeats <= 0) throw std::runtime_error("invalid arguments");

    Matrix A(n), B(n), ref(n), tiled(n), unrolled(n);
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    for (auto& v : A.x) v = dist(rng);
    for (auto& v : B.x) v = dist(rng);

    matmul_naive(A, B, ref);
    matmul_tiled(A, B, tiled, tile);
    matmul_tiled_unrolled(A, B, unrolled, tile);
    const float d1 = max_abs_diff(ref, tiled), d2 = max_abs_diff(ref, unrolled);
    if (d1 > 1e-4f || d2 > 1e-4f) throw std::runtime_error("correctness check failed");
    if (check_only) { std::cout << "correctness: PASS\n"; return 0; }

    const double t_naive = seconds([&] { matmul_naive(A, B, ref); }, repeats);
    const double t_tiled = seconds([&] { matmul_tiled(A, B, tiled, tile); }, repeats);
    const double t_unrolled = seconds([&] { matmul_tiled_unrolled(A, B, unrolled, tile); }, repeats);

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "n=" << n << " tile=" << tile << " repeats=" << repeats << "\n";
    std::cout << "variant,seconds,speedup_vs_naive\n";
    std::cout << "naive," << t_naive << ",1.000x\n";
    std::cout << "tiled," << t_tiled << "," << t_naive / t_tiled << "x\n";
    std::cout << "tiled_unrolled," << t_unrolled << "," << t_naive / t_unrolled << "x\n";
    std::cout << "max_abs_error_tiled=" << d1 << " max_abs_error_unrolled=" << d2 << "\n";
}
