#include <algorithm>
#include <benchmark/benchmark.h>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <immintrin.h>
#include <random>
#include <spdlog/spdlog.h>
#include <vector>

const int N = 1e7;

std::vector<float> x_vec(N);
std::vector<float> y_vec(N);

void init_data()
{
    static std::random_device               rd;
    static std::mt19937                     gen(rd());
    static std::uniform_real_distribution<> dis(float(1e-6), float(1e6));
    std::generate(x_vec.begin(), x_vec.end(), [&]() { return dis(gen); });
    for (int i = 0; i < N; i++)
    {
        y_vec[i] = 0.0f;
    }
}

double check_error(const std::vector<float>& x_vec, const std::vector<float>& y_vec)
{
    double err = 0.0;
    for (int i = 0; i < N; i++)
    {
        err += std::abs(y_vec[i] - 1.0f / std::sqrt(x_vec[i]));
    }
    return err;
}

class Strategy
{
public:
    virtual void operator()() = 0;
};

class StdStrategy : public Strategy
{
public:
    void operator()() override
    {
        for (int i = 0; i < N; i++)
        {
            float sqrt_x = std::sqrt(x_vec[i]);
            benchmark::DoNotOptimize(sqrt_x);
            y_vec[i] = 1.0f / sqrt_x;
        }
    }
};

class SSEStrategy : public Strategy
{
public:
    void operator()() override
    {
        for (int i = 0; i < N; i += 4)
        {
            __m128 x = _mm_loadu_ps(x_vec.data() + i);
            __m128 y = _mm_rsqrt_ps(x);
            _mm_storeu_ps(y_vec.data() + i, y);
        }
    }
};

class AVXStrategy : public Strategy
{
public:
    void operator()() override
    {
        for (int i = 0; i < N; i += 8)
        {
            __m256 x = _mm256_loadu_ps(x_vec.data() + i);
            __m256 y = _mm256_rsqrt_ps(x);
            _mm256_storeu_ps(y_vec.data() + i, y);
        }
    }
};

class Qrsqrt01Strategy : public Strategy
{
public:
    void operator()() override
    {
        auto q_rsqrt = [](float x) {
            int32_t     i;
            float       x2, y;
            const float threehalfs = 1.5f;

            x2 = x * 0.5f;
            y  = x;
            i  = *(int32_t*)&y;         // evil floating point bit hack
            i  = 0x5f3759df - (i >> 1); // magic number
            y  = *(float*)&i;
            y  = y * (threehalfs - (x2 * y * y)); // 1st iteration
            // y  = y * (threehalfs - (x2 * y * y)); // 2nd iteration
            return y;
        };
        for (int i = 0; i < N; i++)
        {
            y_vec[i] = q_rsqrt(x_vec[i]);
        }
    }
};

class Qrsqrt02Strategy : public Strategy
{
public:
    void operator()() override
    {
        auto q_rsqrt = [](float x) {
            int32_t     i;
            float       x2, y;
            const float threehalfs = 1.5f;

            x2 = x * 0.5f;
            y  = x;
            i  = *(int32_t*)&y;         // evil floating point bit hack
            i  = 0x5f3759df - (i >> 1); // magic number
            y  = *(float*)&i;
            // y  = y * (threehalfs - (x2 * y * y)); // 1st iteration
            // y  = y * (threehalfs - (x2 * y * y)); // 2nd iteration
            return y;
        };
        for (int i = 0; i < N; i++)
        {
            y_vec[i] = q_rsqrt(x_vec[i]);
        }
    }
};

class Qrsqrt03Strategy : public Strategy
{
public:
    void operator()() override
    {
        auto q_rsqrt = [](float x) {
            int32_t     i;
            float       x2, y;
            const float threehalfs = 1.5f;

            x2 = x * 0.5f;
            y  = x;
            i  = *(int32_t*)&y;         // evil floating point bit hack
            i  = 0x5f3759df - (i >> 1); // magic number
            y  = *(float*)&i;
            y  = y * (threehalfs - (x2 * y * y)); // 1st iteration
            y  = y * (threehalfs - (x2 * y * y)); // 2nd iteration
            return y;
        };
        for (int i = 0; i < N; i++)
        {
            y_vec[i] = q_rsqrt(x_vec[i]);
        }
    }
};

template<class Strategy>
void BM_Generic(benchmark::State& state)
{
    init_data();
    Strategy strategy;
    auto     start = std::chrono::high_resolution_clock::now();
    for (auto _ : state)
    {
        strategy();
    }
    auto   end              = std::chrono::high_resolution_clock::now();
    auto   duration         = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    double avg_ns_per_multi = double(duration.count()) / (N * state.iterations());
    double e                = check_error(x_vec, y_vec);
    state.SetLabel(fmt::format("avg_ns_per: {:5.2f} ns, Error: {:5.2f}", avg_ns_per_multi, e));
}

// NOLINTBEGIN
#define REGISTER_BENCHMARK(Strategy, MName) BENCHMARK(BM_Generic<Strategy>)->Name(MName)->Unit(benchmark::kMillisecond)
// NOLINTEND

REGISTER_BENCHMARK(StdStrategy, "std");
REGISTER_BENCHMARK(SSEStrategy, "sse");
REGISTER_BENCHMARK(AVXStrategy, "avx");
REGISTER_BENCHMARK(Qrsqrt01Strategy, "qrsqrt01");
REGISTER_BENCHMARK(Qrsqrt02Strategy, "qrsqrt02");
REGISTER_BENCHMARK(Qrsqrt03Strategy, "qrsqrt03");

BENCHMARK_MAIN();