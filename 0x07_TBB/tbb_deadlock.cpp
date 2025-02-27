#include <vector>
#include <cmath>
#include <tbb/tbb.h>
#include <benchmark/benchmark.h>

const int N = 1<<20;
const int P = 1<<10;
std::vector<int> a(N);

// void BM_for_seq(benchmark::State& state) {
//     for (auto _ : state) {
//         for (int i = 0; i < N; i++) {
//             a[i] = std::sin(i);
//         }
//     }
// }
// BENCHMARK(BM_for_seq)->Unit(benchmark::kMillisecond);

// void BM_for_parallel(benchmark::State& state) {
//     for (auto _ : state) {
//         tbb::parallel_for(0, N, [](int i) {
//             a[i] = std::sin(i);
//         });
//     }
// }
// BENCHMARK(BM_for_parallel)->Unit(benchmark::kMillisecond);

// void BM_for_parallel_range(benchmark::State& state) {
//     for (auto _ : state) {
//         tbb::parallel_for(tbb::blocked_range<int>(0, N), [](const tbb::blocked_range<int>& r) {
//             for (int i = r.begin(); i != r.end(); ++i) {
//                 a[i] = std::sin(i);
//             }
//         });
//     }
// }
// BENCHMARK(BM_for_parallel_range)->Unit(benchmark::kMillisecond);

void BM_2for_parallel_01(benchmark::State& state) {
    // std::mutex mtx;
    std::recursive_mutex recursive_mtx;
    for (auto _ : state) {
        tbb::parallel_for(0, P, [&](int i) {
            // 死锁：内部 for 循环有可能“窃取”到另一个外部 for 循环的任务，从而导致 mutex 被重复上锁
            // std::lock_guard<std::mutex> lock(mtx); 
            // 1. std::recursive_mutex
            std::lock_guard<std::recursive_mutex> lock(recursive_mtx);
            tbb::parallel_for(0, P, [&](int j) {
                a[i*P+j] = std::sin(i*P+j);
            });
        });
    }
}
BENCHMARK(BM_2for_parallel_01)->Unit(benchmark::kMillisecond);

void BM_2for_parallel_02(benchmark::State& state) {
    std::mutex mtx;
    // std::recursive_mutex recursive_mtx;
    for (auto _ : state) {
        tbb::parallel_for(0, P, [&](int i) {
            // 死锁：内部 for 循环有可能“窃取”到另一个外部 for 循环的任务，从而导致 mutex 被重复上锁
            std::lock_guard<std::mutex> lock(mtx); 
            // 1. std::recursive_mutex
            // std::lock_guard<std::recursive_mutex> lock(recursive_mtx);
            // 2. tbb::task_arena
            tbb::task_arena arena;
            arena.execute([&]() {
                tbb::parallel_for(0, P, [&](int j) {
                    a[i*P+j] = std::sin(i*P+j);
                });
            });
        });
    }
}
BENCHMARK(BM_2for_parallel_02)->Unit(benchmark::kMillisecond);

void BM_2for_parallel_03(benchmark::State& state) {
    std::mutex mtx;
    for (auto _ : state) {
        tbb::parallel_for(0, P, [&](int i) {
            // 死锁：内部 for 循环有可能“窃取”到另一个外部 for 循环的任务，从而导致 mutex 被重复上锁
            std::lock_guard<std::mutex> lock(mtx); 
            // 1. std::recursive_mutex
            // std::lock_guard<std::recursive_mutex> lock(recursive_mtx);
            // 2. tbb::task_arena
            // tbb::task_arena arena;
            // 3. tbb::this_task_arena::isolate
            tbb::this_task_arena::isolate([&]() {
                tbb::parallel_for(0, P, [&](int j) {
                    a[i*P+j] = std::sin(i*P+j);
                });
            });
        });
    }
}
BENCHMARK(BM_2for_parallel_03)->Unit(benchmark::kMillisecond);

void BM_2for_parallel_03_spin_mutex(benchmark::State& state) {
    // std::mutex mtx;
    tbb::spin_mutex spin_mtx;
    for (auto _ : state) {
        tbb::parallel_for(0, P, [&](int i) {
            // 死锁：内部 for 循环有可能“窃取”到另一个外部 for 循环的任务，从而导致 mutex 被重复上锁
            // std::lock_guard<std::mutex> lock(mtx); 
            // 1. std::recursive_mutex
            // std::lock_guard<std::recursive_mutex> lock(recursive_mtx);
            // 2. tbb::task_arena
            // tbb::task_arena arena;
            // 3. tbb::this_task_arena::isolate
            std::lock_guard<tbb::spin_mutex> lock(spin_mtx);
            tbb::this_task_arena::isolate([&]() {
                tbb::parallel_for(0, P, [&](int j) {
                    a[i*P+j] = std::sin(i*P+j);
                });
            });
        });
    }
}
BENCHMARK(BM_2for_parallel_03_spin_mutex)->Unit(benchmark::kMillisecond);

void BM_2for_parallel_02_spin_mutex(benchmark::State& state) {
    // std::mutex mtx;
    tbb::spin_mutex spin_mtx;
    for (auto _ : state) {
        tbb::parallel_for(0, P, [&](int i) {
            // 死锁：内部 for 循环有可能“窃取”到另一个外部 for 循环的任务，从而导致 mutex 被重复上锁
            // std::lock_guard<std::mutex> lock(mtx); 
            // 1. std::recursive_mutex
            // std::lock_guard<std::recursive_mutex> lock(recursive_mtx);
            // 2. tbb::task_arena
            tbb::task_arena arena;
            arena.execute([&]() {
                tbb::parallel_for(0, P, [&](int j) {
                    a[i*P+j] = std::sin(i*P+j);
                });
            });
        });
    }
}
BENCHMARK(BM_2for_parallel_02_spin_mutex)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();