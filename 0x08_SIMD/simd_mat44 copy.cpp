#include <Eigen/Dense>
#include <benchmark/benchmark.h>

#if defined(_WIN32) || defined(__x86_64__)
#include <immintrin.h>  // SSE头文件
#else
#include <arm_neon.h>   // NEON头文件
#endif

const int Iter = int(1e3);
const int N = int(4e3);

using mat44_row = Eigen::Matrix<float, 4, 4, Eigen::RowMajor>;
using mat44_col = Eigen::Matrix<float, 4, 4, Eigen::ColMajor>; // default

std::vector<mat44_row> Ar_vec(N), Br_vec(N);
std::vector<mat44_col> Ac_vec(N), Bc_vec(N);
std::vector<mat44_col> C_vec(N);

void init_mat44_row_vec(std::vector<mat44_row>& vec) {
    std::srand(time(nullptr));
    for (int i = 0; i < N; i++) vec[i] = mat44_row::Random(4, 4);
}

void init_mat44_col_vec(std::vector<mat44_col>& vec) {
    std::srand(time(nullptr));
    for (int i = 0; i < N; i++) vec[i] = mat44_col::Random(4, 4);
}

template <typename T1, typename T2>
double check_C_error(const std::vector<T1>& A_vec, const std::vector<T2>& B_vec) {
    double e = 0;
    for (int i = 0; i < N; i++) e += (C_vec[i] - (A_vec[i] * B_vec[i])).norm();
    return e;
}

// =============================== RowMajor * RowMajor ===============================

// Sequential, A rowMajor, B rowMajor
void BM_MatMulti_ArBr_seq(benchmark::State& state) {
    auto A = Ar_vec;
    auto B = Br_vec;
    auto C = C_vec;
    init_mat44_row_vec(A);
    init_mat44_row_vec(B);
    auto mat_multi = [&]() {
        for (int i = 0; i < N; i++) {
            C[i] = mat44_row::Zero(4, 4);
            for (int di = 0; di < 4; di++) {
                for (int dj = 0; dj < 4; dj++) {
                    C_vec[i](di, dj) = 0;
                    for (int dk = 0; dk < 4; dk++)
                        C[i](di, dj) += A[i](di, dk) * B[i](dk, dj);
                }
            }
        }
    };
    for (auto _ : state) {
        mat_multi();
    }
    double e = check_C_error(A, B);
    state.SetLabel(std::string("Error: " + std::to_string(e)));
}
BENCHMARK(BM_MatMulti_ArBr_seq)->Unit(benchmark::kNanosecond)->Iterations(Iter);

// Eigen, A rowMajor, B rowMajor
void BM_MatMulti_ArBr_eigen(benchmark::State& state) {
    auto A = Ar_vec;
    auto B = Br_vec;
    auto C = C_vec;
    init_mat44_row_vec(A);
    init_mat44_row_vec(B);
    auto mat_multi = [&]() {
        for (int i = 0; i < N; i++) {
            C[i] = (A[i] * B[i]).eval();
        }
    };
    for (auto _ : state) {
        mat_multi();
    }
    double e = check_C_error(A, B);
    state.SetLabel(std::string("Error: " + std::to_string(e)));
}
BENCHMARK(BM_MatMulti_ArBr_eigen)->Unit(benchmark::kNanosecond)->Iterations(Iter);

// SIMD, A rowMajor, B rowMajor
void BM_MatMulti_ArBr_simd(benchmark::State& state) {
    auto A = Ar_vec;
    auto B = Br_vec;
    auto C = C_vec;
    init_mat44_row_vec(A);
    init_mat44_row_vec(B);
    auto mat_multi_simd = [&]() {
        for (int i = 0; i < N; i++) {
            for (int di = 0; di < 4; di++) {
                for (int dj = 0; dj < 4; dj++) {
                    __m128 sum = _mm_setzero_ps();
                    __m128 a = _mm_set_ps(A[i](di, 0), A[i](di, 1), A[i](di, 2), A[i](di, 3));
                    __m128 b = _mm_set_ps(B[i](0, dj), B[i](1, dj), B[i](2, dj), B[i](3, dj));
                    sum = _mm_add_ps(sum, _mm_mul_ps(a, b));
                    sum = _mm_add_ps(sum, _mm_mul_ps(a, b));
                    C[i](di, dj) = _mm_cvtss_f32(sum);
                }
            }
        }
    };
    for (auto _ : state) {
        mat_multi_simd();
    }
    double e = check_C_error(A, B);
    state.SetLabel(std::string("Error: " + std::to_string(e)));
}
BENCHMARK(BM_MatMulti_ArBr_simd)->Unit(benchmark::kNanosecond)->Iterations(Iter);



// =============================== RowMajor * ColMajor ===============================

// Sequential, A rowMajor, B colMajor
void BM_MatMulti_ArBc_seq(benchmark::State& state) {
    auto A = Ar_vec;
    auto B = Bc_vec;
    auto C = C_vec;
    init_mat44_row_vec(A);
    init_mat44_col_vec(B);
    auto mat_multi = [&]() {
        for (int i = 0; i < N; i++) {
            C[i] = mat44_row::Zero(4, 4);
            for (int di = 0; di < 4; di++) {
                for (int dj = 0; dj < 4; dj++) {
                    C_vec[i](di, dj) = 0;
                    for (int dk = 0; dk < 4; dk++)
                        C[i](di, dj) += A[i](di, dk) * B[i](dk, dj);
                }
            }
        }
    };
    for (auto _ : state) {
        mat_multi();
    }
    double e = check_C_error(A, B);
    state.SetLabel(std::string("Error: " + std::to_string(e)));
}
BENCHMARK(BM_MatMulti_ArBc_seq)->Unit(benchmark::kNanosecond)->Iterations(Iter);

// Eigen, A rowMajor, B colMajor
void BM_MatMulti_ArBc_eigen(benchmark::State& state) {
    auto A = Ar_vec;
    auto B = Bc_vec;
    auto C = C_vec;
    init_mat44_row_vec(A);
    init_mat44_col_vec(B);
    auto mat_multi = [&]() {
        for (int i = 0; i < N; i++) {
            C[i] = (A[i] * B[i]).eval();
        }
    };
    for (auto _ : state) {
        mat_multi();
    }
    double e = check_C_error(A, B);
    state.SetLabel(std::string("Error: " + std::to_string(e)));
}
BENCHMARK(BM_MatMulti_ArBc_eigen)->Unit(benchmark::kNanosecond)->Iterations(Iter);

// SIMD, A rowMajor, B colMajor
void BM_MatMulti_ArBc_simd(benchmark::State& state) {
    auto A = Ar_vec;
    auto B = Bc_vec;
    auto C = C_vec;
    init_mat44_row_vec(A);
    init_mat44_col_vec(B);
    auto mat_multi_simd = [&]() {
        for (int i = 0; i < N; i++) {
            for (int di = 0; di < 4; di++) {
                for (int dj = 0; dj < 4; dj++) {
                    __m128 sum = _mm_setzero_ps();
                    __m128 a = _mm_set_ps(A[i](di, 0), A[i](di, 1), A[i](di, 2), A[i](di, 3));
                    __m128 b = _mm_set_ps(B[i](0, dj), B[i](1, dj), B[i](2, dj), B[i](3, dj));
                    sum = _mm_add_ps(sum, _mm_mul_ps(a, b));
                    sum = _mm_add_ps(sum, _mm_mul_ps(a, b));
                    C[i](di, dj) = _mm_cvtss_f32(sum);
                }
            }
        }
    };
    for (auto _ : state) {
        mat_multi_simd();
    }
    double e = check_C_error(A, B);
    state.SetLabel(std::string("Error: " + std::to_string(e)));
}
BENCHMARK(BM_MatMulti_ArBc_simd)->Unit(benchmark::kNanosecond)->Iterations(Iter);



// =============================== ColMajor * RowMajor ===============================

// Sequential, A colMajor, B rowMajor
void BM_MatMulti_AcBr_seq(benchmark::State& state) {
    auto A = Ac_vec;
    auto B = Br_vec;
    auto C = C_vec;
    init_mat44_col_vec(A);
    init_mat44_row_vec(B);
    auto mat_multi = [&]() {
        for (int i = 0; i < N; i++) {
            C[i] = mat44_row::Zero(4, 4);
            for (int di = 0; di < 4; di++) {
                for (int dj = 0; dj < 4; dj++) {
                    C_vec[i](di, dj) = 0;
                    for (int dk = 0; dk < 4; dk++)
                        C[i](di, dj) += A[i](di, dk) * B[i](dk, dj);
                }
            }
        }
    };
    for (auto _ : state) {
        mat_multi();
    }
    double e = check_C_error(A, B);
    state.SetLabel(std::string("Error: " + std::to_string(e)));
}
BENCHMARK(BM_MatMulti_AcBr_seq)->Unit(benchmark::kNanosecond)->Iterations(Iter);

// Eigen, A colMajor, B rowMajor
void BM_MatMulti_AcBr_eigen(benchmark::State& state) {
    auto A = Ac_vec;
    auto B = Br_vec;
    auto C = C_vec;
    init_mat44_col_vec(A);
    init_mat44_row_vec(B);
    auto mat_multi = [&]() {
        for (int i = 0; i < N; i++) {
            C[i] = (A[i] * B[i]).eval();
        }
    };
    for (auto _ : state) {
        mat_multi();
    }
    double e = check_C_error(A, B);
    state.SetLabel(std::string("Error: " + std::to_string(e)));
}
BENCHMARK(BM_MatMulti_AcBr_eigen)->Unit(benchmark::kNanosecond)->Iterations(Iter);

// SIMD, A colMajor, B rowMajor
void BM_MatMulti_AcBr_simd(benchmark::State& state) {
    auto A = Ac_vec;
    auto B = Br_vec;
    auto C = C_vec;
    init_mat44_col_vec(A);
    init_mat44_row_vec(B);
    auto mat_multi_simd = [&]() {
        for (int i = 0; i < N; i++) {
            for (int di = 0; di < 4; di++) {
                for (int dj = 0; dj < 4; dj++) {
                    __m128 sum = _mm_setzero_ps();
                    __m128 a = _mm_set_ps(A[i](di, 0), A[i](di, 1), A[i](di, 2), A[i](di, 3));
                    __m128 b = _mm_set_ps(B[i](0, dj), B[i](1, dj), B[i](2, dj), B[i](3, dj));
                    sum = _mm_add_ps(sum, _mm_mul_ps(a, b));
                    sum = _mm_add_ps(sum, _mm_mul_ps(a, b));
                    C[i](di, dj) = _mm_cvtss_f32(sum);
                }
            }
        }
    };
    for (auto _ : state) {
        mat_multi_simd();
    }
    double e = check_C_error(A, B);
    state.SetLabel(std::string("Error: " + std::to_string(e)));
}
BENCHMARK(BM_MatMulti_AcBr_simd)->Unit(benchmark::kNanosecond)->Iterations(Iter);


// =============================== ColMajor * ColMajor ===============================

// Sequential, A colMajor, B colMajor
void BM_MatMulti_AcBc_seq(benchmark::State& state) {
    auto A = Ac_vec;
    auto B = Bc_vec;
    auto C = C_vec;
    init_mat44_col_vec(A);
    init_mat44_col_vec(B);
    auto mat_multi = [&]() {
        for (int i = 0; i < N; i++) {
            C[i] = mat44_row::Zero(4, 4);
            for (int di = 0; di < 4; di++) {
                for (int dj = 0; dj < 4; dj++) {
                    C_vec[i](di, dj) = 0;
                    for (int dk = 0; dk < 4; dk++)
                        C[i](di, dj) += A[i](di, dk) * B[i](dk, dj);
                }
            }
        }
    };
    for (auto _ : state) {
        mat_multi();
    }
    double e = check_C_error(A, B);
    state.SetLabel(std::string("Error: " + std::to_string(e)));
}
BENCHMARK(BM_MatMulti_AcBc_seq)->Unit(benchmark::kNanosecond)->Iterations(Iter);

// Eigen, A colMajor, B colMajor
void BM_MatMulti_AcBc_eigen(benchmark::State& state) {
    auto A = Ac_vec;
    auto B = Bc_vec;
    auto C = C_vec;
    init_mat44_col_vec(A);
    init_mat44_col_vec(B);
    auto mat_multi = [&]() {
        for (int i = 0; i < N; i++) {
            C[i] = (A[i] * B[i]).eval();
        }
    };
    for (auto _ : state) {
        mat_multi();
    }
    double e = check_C_error(A, B);
    state.SetLabel(std::string("Error: " + std::to_string(e)));
}
BENCHMARK(BM_MatMulti_AcBc_eigen)->Unit(benchmark::kNanosecond)->Iterations(Iter);

// SIMD, A colMajor, B colMajor
void BM_MatMulti_AcBc_simd(benchmark::State& state) {
    auto A = Ac_vec;
    auto B = Bc_vec;
    auto C = C_vec;
    init_mat44_col_vec(A);
    init_mat44_col_vec(B);
    auto mat_multi_simd = [&]() {
        for (int i = 0; i < N; i++) {
            for (int di = 0; di < 4; di++) {
                for (int dj = 0; dj < 4; dj++) {
                    __m128 sum = _mm_setzero_ps();
                    __m128 a = _mm_set_ps(A[i](di, 0), A[i](di, 1), A[i](di, 2), A[i](di, 3));
                    __m128 b = _mm_set_ps(B[i](0, dj), B[i](1, dj), B[i](2, dj), B[i](3, dj));
                    sum = _mm_add_ps(sum, _mm_mul_ps(a, b));
                    sum = _mm_add_ps(sum, _mm_mul_ps(a, b));
                    C[i](di, dj) = _mm_cvtss_f32(sum);
                }
            }
        }
    };
    for (auto _ : state) {
        mat_multi_simd();
    }
    double e = check_C_error(A, B);
    state.SetLabel(std::string("Error: " + std::to_string(e)));
}
BENCHMARK(BM_MatMulti_AcBc_simd)->Unit(benchmark::kNanosecond)->Iterations(Iter);


BENCHMARK_MAIN();