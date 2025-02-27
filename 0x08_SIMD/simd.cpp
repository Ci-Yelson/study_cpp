#include <algorithm>
#include <vector>
#include <benchmark/benchmark.h>

#if defined(_WIN32) || defined(__x86_64__)
#include <x86intrin.h>  // SSE头文件 [TODO]
#else
#include <arm_neon.h>   // NEON头文件
#endif

// const int N = 4, M = 4;
const int N = 4 * 2, M = 4 * 2;
std::vector<std::vector<float>> a(N, std::vector<float>(M));
std::vector<std::vector<float>> b(M, std::vector<float>(N));
std::vector<std::vector<float>> c(N, std::vector<float>(N));

void BM_MatMulti_seq(benchmark::State& state) {
    std::srand(std::time(nullptr));
    for (int i = 0; i < N; i++)
        std::generate(a[i].begin(), a[i].end(), std::rand);
    for (int i = 0; i < M; i++)
        std::generate(b[i].begin(), b[i].end(), std::rand);

    auto mat_multi = [&]() {
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                for (int k = 0; k < M; k++)
                    c[i][j] += a[i][k] * b[k][j];
    };

    for (auto _ : state) {
        mat_multi();
    }
}
BENCHMARK(BM_MatMulti_seq)->Unit(benchmark::kNanosecond);

void BM_MatMulti_simd(benchmark::State& state) {
    std::srand(std::time(nullptr));
    for (int i = 0; i < N; i++)
        std::generate(a[i].begin(), a[i].end(), std::rand);
    for (int i = 0; i < M; i++)
        std::generate(b[i].begin(), b[i].end(), std::rand);

    auto mat_multi_simd = [&]() {
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
#if defined(_WIN32) || defined(__x86_64__)
                // SSE实现
                __m128 sum_vec = _mm_setzero_ps();
                for (int k = 0; k < M; k += 4) {
                    __m128 a_vec = _mm_loadu_ps(&a[i][k]);
                    __m128 b_vec = _mm_loadu_ps(&b[k][j]);
                    sum_vec = _mm_add_ps(sum_vec, _mm_mul_ps(a_vec, b_vec));
                }
                // 水平相加
                sum_vec = _mm_hadd_ps(sum_vec, sum_vec);
                sum_vec = _mm_hadd_ps(sum_vec, sum_vec);
                _mm_store_ss(&c[i][j], sum_vec);
#else
                // NEON实现
                float32x4_t sum_vec = vdupq_n_f32(0.0f);
                for (int k = 0; k < M; k += 4) {
                    float32x4_t a_vec = vld1q_f32(&a[i][k]);
                    float32x4_t b_vec = vld1q_f32(&b[k][j]);
                    sum_vec = vmlaq_f32(sum_vec, a_vec, b_vec);
                }
                c[i][j] = vaddvq_f32(sum_vec);
#endif
            }
        }
    };

    for (auto _ : state) {
        mat_multi_simd();
    }
}
BENCHMARK(BM_MatMulti_simd)->Unit(benchmark::kNanosecond);

BENCHMARK_MAIN();