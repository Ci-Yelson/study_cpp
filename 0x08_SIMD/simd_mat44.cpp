#include <Eigen/Dense>
#include <chrono>
#include <spdlog/spdlog.h>
#include <benchmark/benchmark.h>

#if defined(_WIN32) || defined(__x86_64__)
#include <immintrin.h>  // SSE头文件
#else
#include <arm_neon.h>   // NEON头文件
#endif

const int N = int(4e3);

using mat44_row = Eigen::Matrix<float, 4, 4, Eigen::RowMajor>;
using mat44_col = Eigen::Matrix<float, 4, 4, Eigen::ColMajor>; // default
using matC_type = std::vector<mat44_row>;

std::vector<mat44_row> Ar_vec(N), Br_vec(N);
std::vector<mat44_col> Ac_vec(N), Bc_vec(N);
matC_type C_vec(N);

template <typename T>
void init_mat44_vec(std::vector<T>& vec) {
    std::srand(time(nullptr));
    for (int i = 0; i < N; i++) vec[i] = T::Random(4, 4);
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
    virtual void operator()(const std::vector<MatA>& A, const std::vector<MatB>& B, matC_type& C) = 0;
};

// 顺序计算策略
template <typename MatA, typename MatB>
class SequentialStrategy : public MatMultiStrategy<MatA, MatB> {
public:
    void operator()(const std::vector<MatA>& A, const std::vector<MatB>& B, matC_type& C) override {
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
    void operator()(const std::vector<MatA>& A, const std::vector<MatB>& B, matC_type& C) override {
        for (int i = 0; i < N; i++) {
            C[i] = (A[i] * B[i]).eval();
        }
    }
};

// SIMD计算策略 - 01
template <typename MatA, typename MatB>
class SIMDStrategy_01 : public MatMultiStrategy<MatA, MatB> {
public:
    void operator()(const std::vector<MatA>& A, const std::vector<MatB>& B, matC_type& C) override {
        for (int i = 0; i < N; i++) {
            for (int di = 0; di < 4; di++) {
                for (int dj = 0; dj < 4; dj++) {
                    __m128 a = _mm_setr_ps(A[i](di, 0), A[i](di, 1), A[i](di, 2), A[i](di, 3));
                    __m128 b = _mm_setr_ps(B[i](0, dj), B[i](1, dj), B[i](2, dj), B[i](3, dj));
                    __m128 sum = _mm_mul_ps(a, b);
                    sum = _mm_hadd_ps(sum, sum);
                    sum = _mm_hadd_ps(sum, sum);
                    C[i](di, dj) = _mm_cvtss_f32(sum);
                }
            }
        }
    }
};

// SIMD计算策略 - reg
template <typename MatA, typename MatB>
class SIMDStrategy_reg : public MatMultiStrategy<MatA, MatB> {
public:
    void operator()(const std::vector<MatA>& A, const std::vector<MatB>& B, matC_type& C) override {
        for (int i = 0; i < N; i++) {
            __m128 b0 = _mm_setr_ps(B[i](0, 0), B[i](1, 0), B[i](2, 0), B[i](3, 0));
            __m128 b1 = _mm_setr_ps(B[i](0, 1), B[i](1, 1), B[i](2, 1), B[i](3, 1));
            __m128 b2 = _mm_setr_ps(B[i](0, 2), B[i](1, 2), B[i](2, 2), B[i](3, 2));
            __m128 b3 = _mm_setr_ps(B[i](0, 3), B[i](1, 3), B[i](2, 3), B[i](3, 3));

            for (int row = 0; row < 4; row++) {
                __m128 a = _mm_setr_ps(A[i](row, 0), A[i](row, 1), A[i](row, 2), A[i](row, 3));
                __m128 c0 = _mm_mul_ps(a, b0); // [a00*b00, a01*b10, a02*b20, a03*b30]
                __m128 c1 = _mm_mul_ps(a, b1); // [a00*b01, a01*b11, a02*b21, a03*b31]
                __m128 c2 = _mm_mul_ps(a, b2); // [a00*b02, a01*b12, a02*b22, a03*b32]
                __m128 c3 = _mm_mul_ps(a, b3); // [a00*b03, a01*b13, a02*b23, a03*b33]

                c0 = _mm_hadd_ps(c0, c1); // [a00*b00+a01*b10, a02*b20+a03*b30, a00*b01+a01*b11, a02*b21+a03*b31]
                c2 = _mm_hadd_ps(c2, c3); // [a00*b02+a01*b12, a02*b22+a03*b32, a00*b03+a01*b13, a02*b23+a03*b33]
                c0 = _mm_hadd_ps(c0, c2); // [a00*b00+a01*b10+a02*b20+a03*b30, a00*b01+a01*b11+a02*b21+a03*b31, a00*b02+a01*b12+a02*b22+a03*b32, a00*b03+a01*b13+a02*b23+a03*b33]
                _mm_store_ps(C[i].data() + row * 4, c0);
            }
        }
    }
};

// SIMD计算策略 - reg_fmad
template <typename MatA, typename MatB>
class SIMDStrategy_reg_fmad : public MatMultiStrategy<MatA, MatB> {
public:
    void operator()(const std::vector<MatA>& A, const std::vector<MatB>& B, matC_type& C) override {
        for (int i = 0; i < N; i++) {
            // Col Version
            // A x B = [A * bc0, A * bc1, A * bc2, A * bc3]
            //       = [(ac0 * b00 + ac1 * b10 + ac2 * b20 + ac3 * b30), // A每列与B第一列每个元素相乘 -> C第一列
            //          (ac0 * b01 + ac1 * b11 + ac2 * b21 + ac3 * b31), // A每列与B第二列每个元素相乘 -> C第二列
            //          (ac0 * b02 + ac1 * b12 + ac2 * b22 + ac3 * b32), // A每列与B第三列每个元素相乘 -> C第三列
            //          (ac0 * b03 + ac1 * b13 + ac2 * b23 + ac3 * b33)] // A每列与B第四列每个元素相乘 -> C第四列
            //       = [fmad(ac0, {b00}, fmad(ac1, {b10}, fmad(ac2, {b20}, mad(ac3, {b30}))),
            //          fmad(ac0, {b01}, fmad(ac1, {b11}, fmad(ac2, {b21}, mad(ac3, {b31}))),
            //          fmad(ac0, {b02}, fmad(ac1, {b12}, fmad(ac2, {b22}, mad(ac3, {b32}))),
            //          fmad(ac0, {b03}, fmad(ac1, {b13}, fmad(ac2, {b23}, mad(ac3, {b33})))]

            // for (int colC = 0; colC < 4; colC++) {
            //     __m128 c = _mm_setzero_ps();
            //     for (int colA = 0; colA < 4; colA++) {
            //         __m128 a = _mm_setr_ps(A[i](0, colA), A[i](1, colA), A[i](2, colA), A[i](3, colA));
            //         c = _mm_fmadd_ps(a, _mm_set1_ps(B[i](colA, colC)), c);
            //     }
            //     float tc[4];
            //     _mm_store_ps(tc, c);
            //     C[i](0, colC) = tc[0];
            //     C[i](1, colC) = tc[1];
            //     C[i](2, colC) = tc[2];
            //     C[i](3, colC) = tc[3];
            // }

            // Row Version
            // A x B = [ar0 * B, ar1 * B, ar2 * B, ar3 * B]^T
            //       = [(a00 * bc0 + a01 * bc1 + a02 * bc2 + a03 * bc3), // B每列与A第一行每个元素相乘 -> C第一行
            //          (a10 * bc0 + a11 * bc1 + a12 * bc2 + a13 * bc3), // B每列与A第二行每个元素相乘 -> C第二行
            //          (a20 * bc0 + a21 * bc1 + a22 * bc2 + a23 * bc3), // B每列与A第三行每个元素相乘 -> C第三行
            //          (a30 * bc0 + a31 * bc1 + a32 * bc2 + a33 * bc3)] // B每列与A第四行每个元素相乘 -> C第四行

            // for (int rowC = 0; rowC < 4; rowC++) {
            //     __m128 c = _mm_setzero_ps();
            //     for (int rowB = 0; rowB < 4; rowB++) {
            //         __m128 b = _mm_setr_ps(B[i](rowB, 0), B[i](rowB, 1), B[i](rowB, 2), B[i](rowB, 3));
            //         c = _mm_fmadd_ps(_mm_set1_ps(A[i](rowC, rowB)), b, c);
            //     }
            //     _mm_store_ps(&C[i](rowC, 0), c);
            // }
            __m128 br0 = _mm_setr_ps(B[i](0, 0), B[i](0, 1), B[i](0, 2), B[i](0, 3));
            __m128 br1 = _mm_setr_ps(B[i](1, 0), B[i](1, 1), B[i](1, 2), B[i](1, 3));
            __m128 br2 = _mm_setr_ps(B[i](2, 0), B[i](2, 1), B[i](2, 2), B[i](2, 3));
            __m128 br3 = _mm_setr_ps(B[i](3, 0), B[i](3, 1), B[i](3, 2), B[i](3, 3));
            for (int rowC = 0; rowC < 4; rowC++) {
                __m128 c = _mm_setzero_ps();
                c = _mm_fmadd_ps(_mm_set1_ps(A[i](rowC, 0)), br0, c);
                c = _mm_fmadd_ps(_mm_set1_ps(A[i](rowC, 1)), br1, c);
                c = _mm_fmadd_ps(_mm_set1_ps(A[i](rowC, 2)), br2, c);
                c = _mm_fmadd_ps(_mm_set1_ps(A[i](rowC, 3)), br3, c);
                _mm_store_ps(&C[i](rowC, 0), c);
            }
            
        }
    }
};

template <typename MatT, typename T1, typename T2>
const auto& get_Mat_vector(std::vector<T1>& Mr_vec, std::vector<T2>& Mc_vec) {
    if constexpr (std::is_same_v<MatT, mat44_row>)  {
        init_mat44_vec(Mr_vec);
        return Mr_vec;
    }
    else  {
        init_mat44_vec(Mc_vec);
        return Mc_vec;
    }
}

// 通用Benchmark函数
template <typename MatA, typename MatB, typename Strategy>
void BM_MatMulti_Generic(benchmark::State& state) {
    // 使用类型安全的获取方式
    const auto& A = get_Mat_vector<MatA>(Ar_vec, Ac_vec);
    const auto& B = get_Mat_vector<MatB>(Br_vec, Bc_vec);
    auto& C = C_vec;

    Strategy strategy;
    auto start = std::chrono::high_resolution_clock::now();
    for (auto _ : state) {
        strategy(A, B, C);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    double avg_ns_per_multi = double(duration.count()) / (N * state.iterations());
    double e = check_C_error(A, B);
    state.SetLabel(fmt::format("avg_ns_per_multi: {:5.2f} ns, Error: {:5.2f}", avg_ns_per_multi, e));
}

// 修正后的宏定义
// NOLINTBEGIN
#define REGISTER_BENCHMARK(MatA, MatB, Strategy, MName) \
    BENCHMARK( \
        BM_MatMulti_Generic<MatA, MatB, Strategy<MatA, MatB>> \
    )->Name(MName)->Unit(benchmark::kMillisecond)
// NOLINTEND

// 注册所有测试
REGISTER_BENCHMARK(mat44_row, mat44_row, SequentialStrategy, "ArBr_seq");
REGISTER_BENCHMARK(mat44_row, mat44_row, EigenStrategy, "ArBr_eigen");
REGISTER_BENCHMARK(mat44_row, mat44_row, SIMDStrategy_01, "ArBr_simd01");
REGISTER_BENCHMARK(mat44_row, mat44_row, SIMDStrategy_reg, "ArBr_simd_reg");
REGISTER_BENCHMARK(mat44_row, mat44_row, SIMDStrategy_reg_fmad, "ArBr_simd_reg_fmad");

REGISTER_BENCHMARK(mat44_row, mat44_col, SequentialStrategy, "ArBc_seq");
REGISTER_BENCHMARK(mat44_row, mat44_col, EigenStrategy, "ArBc_eigen");
REGISTER_BENCHMARK(mat44_row, mat44_col, SIMDStrategy_01, "ArBc_simd01");
REGISTER_BENCHMARK(mat44_row, mat44_col, SIMDStrategy_reg, "ArBc_simd_reg");
REGISTER_BENCHMARK(mat44_row, mat44_col, SIMDStrategy_reg_fmad, "ArBc_simd_reg_fmad");

REGISTER_BENCHMARK(mat44_col, mat44_row, SequentialStrategy, "AcBr_seq");
REGISTER_BENCHMARK(mat44_col, mat44_row, EigenStrategy, "AcBr_eigen");
REGISTER_BENCHMARK(mat44_col, mat44_row, SIMDStrategy_01, "AcBr_simd01");
REGISTER_BENCHMARK(mat44_col, mat44_row, SIMDStrategy_reg, "AcBr_simd_reg");
REGISTER_BENCHMARK(mat44_col, mat44_row, SIMDStrategy_reg_fmad, "AcBr_simd_reg_fmad");

REGISTER_BENCHMARK(mat44_col, mat44_col, SequentialStrategy, "AcBc_seq");
REGISTER_BENCHMARK(mat44_col, mat44_col, EigenStrategy, "AcBc_eigen");
REGISTER_BENCHMARK(mat44_col, mat44_col, SIMDStrategy_01, "AcBc_simd01");
REGISTER_BENCHMARK(mat44_col, mat44_col, SIMDStrategy_reg, "AcBc_simd_reg");
REGISTER_BENCHMARK(mat44_col, mat44_col, SIMDStrategy_reg_fmad, "AcBc_simd_reg_fmad");

BENCHMARK_MAIN();