#include <vector>
#include <tbb/tbb.h>
#include <benchmark/benchmark.h>

const int N = 1<<20;
const int P = 1<<10;
std::vector<int> a(N);

int fib_seq(int n) {
    if (n <= 1) return n;
    return fib_seq(n-1) + fib_seq(n-2);
}

void BM_fib_seq(benchmark::State& state) {
    for (auto _ : state) {
        fib_seq(42);
    }
}
BENCHMARK(BM_fib_seq)->Unit(benchmark::kMillisecond);

void BM_fib_task_group(benchmark::State& state) {
    // 需要将fib声明为std::function以支持递归
    std::function<int(int)> fib;
    fib = [&fib](int n) -> int {
        if (n <= 1) return n;
        int r1 = 0, r2 = 0;
        tbb::task_group tg;
        tg.run([&] { r1 = fib(n-1); });
        tg.run([&] { r2 = fib(n-2); });
        tg.wait();
        return r1 + r2;
    };
    for (auto _ : state) {
        fib(42);
    }
}
BENCHMARK(BM_fib_task_group)->Unit(benchmark::kMillisecond);

void BM_fib_parallel_invoke(benchmark::State& state) {
    // 需要将fib声明为std::function以支持递归
    std::function<int(int)> fib;
    fib = [&fib](int n) -> int {
        if (n <= 1) return n;
        int r1 = 0, r2 = 0;
        tbb::parallel_invoke([&] { r1 = fib(n-1); }, [&] { r2 = fib(n-2); });
        return r1 + r2;
    };
    for (auto _ : state) {
        fib(42);
    }
}
BENCHMARK(BM_fib_parallel_invoke)->Unit(benchmark::kMillisecond);

void BM_fib_task_group_cutoff20(benchmark::State& state) {
    // 需要将fib声明为std::function以支持递归
    std::function<int(int)> fib;
    fib = [&fib](int n) -> int {
        if (n <= 20) return fib_seq(n);
        int r1 = 0, r2 = 0;
        tbb::task_group tg;
        tg.run([&] { r1 = fib(n-1); });
        tg.run([&] { r2 = fib(n-2); });
        tg.wait();
        return r1 + r2;
    };
    for (auto _ : state) {
        fib(42);
    }
}
BENCHMARK(BM_fib_task_group_cutoff20)->Unit(benchmark::kMillisecond);

void BM_fib_parallel_invoke_cutoff20(benchmark::State& state) {
    // 需要将fib声明为std::function以支持递归
    std::function<int(int)> fib;
    fib = [&fib](int n) -> int {
        if (n <= 20) return fib_seq(n);
        int r1 = 0, r2 = 0;
        tbb::parallel_invoke([&] { r1 = fib(n-1); }, [&] { r2 = fib(n-2); });
        return r1 + r2;
    };
    for (auto _ : state) {
        fib(42);
    }
}
BENCHMARK(BM_fib_parallel_invoke_cutoff20)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();