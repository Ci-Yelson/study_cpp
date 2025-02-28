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

// 策略模式基类
template <typename MatA, typename MatB>
class MatMultiStrategy {
public:
    virtual void operator()(const std::vector<MatA>& A, const std::vector<MatB>& B, std::vector<mat44_col>& C) = 0;
};

// 顺序计算策略
template <typename MatA, typename MatB>
class SequentialStrategy : public MatMultiStrategy<MatA, MatB> {
public:
    void operator()(const std::vector<MatA>& A, const std::vector<MatB>& B, std::vector<mat44_col>& C) override {
        for (int i = 0; i < N; i++) {
            C[i] = mat44_col::Zero(4, 4);
            for (int di = 0; di < 4; di++) {
                for (int dj = 0; dj < 4; dj++) {
                    for (int dk = 0; dk < 4; dk++)
                        C[i](di, dj) += A[i](di, dk) * B[i](dk, dj);
                }
            }
        }
    }
};

// Eigen计算策略
template <typename MatA, typename MatB>
class EigenStrategy : public MatMultiStrategy<MatA, MatB> {
public:
    void operator()(const std::vector<MatA>& A, const std::vector<MatB>& B, std::vector<mat44_col>& C) override {
        for (int i = 0; i < N; i++) {
            C[i] = (A[i] * B[i]).eval();
        }
    }
};

// SIMD计算策略
template <typename MatA, typename MatB>
class SIMDStrategy : public MatMultiStrategy<MatA, MatB> {
public:
    void operator()(const std::vector<MatA>& A, const std::vector<MatB>& B, std::vector<mat44_col>& C) override {
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
    }
};

// 通用Benchmark函数
template <typename MatA, typename MatB, typename Strategy>
void BM_MatMulti_Generic(benchmark::State& state) {
    auto A = std::is_same<MatA, mat44_row>::value ? Ar_vec : Ac_vec;
    auto B = std::is_same<MatB, mat44_row>::value ? Br_vec : Bc_vec;
    auto C = C_vec;

    if constexpr (std::is_same<MatA, mat44_row>::value) {
        init_mat44_row_vec(A);
    } else {
        init_mat44_col_vec(A);
    }

    if constexpr (std::is_same<MatB, mat44_row>::value) {
        init_mat44_row_vec(B);
    } else {
        init_mat44_col_vec(B);
    }

    Strategy strategy;
    for (auto _ : state) {
        strategy(A, B, C);
    }
    double e = check_C_error(A, B);
    state.SetLabel(std::string("Error: " + std::to_string(e)));
}

// 修改宏定义，显式指定模板参数
#define REGISTER_BENCHMARK(MatA, MatB, Strategy, Name) \
    BENCHMARK(BM_MatMulti_Generic<MatA, MatB, Strategy<MatA, MatB>>)->Name() \
    ->Unit(benchmark::kNanosecond) \
    ->Iterations(Iter)

// 注册所有测试
REGISTER_BENCHMARK(mat44_row, mat44_row, SequentialStrategy, "ArBr_seq");
REGISTER_BENCHMARK(mat44_row, mat44_row, EigenStrategy, "ArBr_eigen");
REGISTER_BENCHMARK(mat44_row, mat44_row, SIMDStrategy, "ArBr_simd");

REGISTER_BENCHMARK(mat44_row, mat44_col, SequentialStrategy, "ArBc_seq");
REGISTER_BENCHMARK(mat44_row, mat44_col, EigenStrategy, "ArBc_eigen");
REGISTER_BENCHMARK(mat44_row, mat44_col, SIMDStrategy, "ArBc_simd");

REGISTER_BENCHMARK(mat44_col, mat44_row, SequentialStrategy, "AcBr_seq");
REGISTER_BENCHMARK(mat44_col, mat44_row, EigenStrategy, "AcBr_eigen");
REGISTER_BENCHMARK(mat44_col, mat44_row, SIMDStrategy, "AcBr_simd");

REGISTER_BENCHMARK(mat44_col, mat44_col, SequentialStrategy, "AcBc_seq");
REGISTER_BENCHMARK(mat44_col, mat44_col, EigenStrategy, "AcBc_eigen");
REGISTER_BENCHMARK(mat44_col, mat44_col, SIMDStrategy, "AcBc_simd");

BENCHMARK_MAIN();