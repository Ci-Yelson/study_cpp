#include <algorithm>
#include <benchmark/benchmark.h>
#include <random>
#include <spdlog/spdlog.h>
#include <unordered_map>
#include <vector>
#include <absl/container/flat_hash_map.h>

const int N = 1e6;  // 测试数据量

// 准备测试数据
std::vector<int64_t> keys(N);
std::vector<int> values(N);

void init_data() {
    static const uint32_t SEED = 42;
    static std::mt19937 gen(SEED);
    static std::uniform_int_distribution<> dis(1, N * 10);
    
    // 生成随机的key和value
    std::generate(keys.begin(), keys.end(), [&]() { return dis(gen); });
    std::generate(values.begin(), values.end(), [&]() { return dis(gen); });

    // 手动构造数据
    // for (int64_t i = 0; i < N; i++) {
    //     keys[i] = i * 107897;
    //     keys[i] = i * 126271;
    //     values[i] = i;
    // }
    // std::shuffle(keys.begin(), keys.end(), gen);
}

class Strategy {
public:
    virtual void insert_test(int64_t k, int v) = 0;
    virtual void lookup_test() = 0;
    virtual void delete_test() = 0;
    virtual ~Strategy() = default;
};

class StdHashMapStrategy : public Strategy {
private:
    std::unordered_map<int64_t, int> map;

public:
    void insert_test(int64_t k, int v) override {
        map.insert({k, v});
    }

    void lookup_test() override {
        for (int i = 0; i < N; i++) {
            benchmark::DoNotOptimize(map.find(keys[i]));
        }
    }

    void delete_test() override {
        for (int i = 0; i < N; i++) {
            map.erase(keys[i]);
        }
        benchmark::DoNotOptimize(map);
    }
};

class AbslHashMapStrategy : public Strategy {
private:
    absl::flat_hash_map<int64_t, int> map;

public:
    void insert_test(int64_t k, int v) override {
        map.insert({k, v});
    }

    void lookup_test() override {
        for (int i = 0; i < N; i++) {
            benchmark::DoNotOptimize(map.find(keys[i]));
        }
    }

    void delete_test() override {
        for (int i = 0; i < N; i++) {
            map.erase(keys[i]);
        }
        benchmark::DoNotOptimize(map);
    }
};

template<class Strategy>
void BM_Insert_MaxInsertTime(benchmark::State& state) {
    init_data();
    Strategy strategy;
    double max_time_insert_ns = 0;
    for (auto _ : state) {
        for (int i = 0; i < N; i++) {
            auto start = std::chrono::high_resolution_clock::now();
            strategy.insert_test(keys[i], values[i]);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            max_time_insert_ns = std::max(max_time_insert_ns, duration.count() * 1.0);
        }
    }
    state.SetLabel(fmt::format("max_time_insert_ns: {}", max_time_insert_ns));
}

template<class Strategy>
void BM_Insert(benchmark::State& state) {
    init_data();
    Strategy strategy;
    for (auto _ : state) {
        for (int i = 0; i < N; i++) {
            strategy.insert_test(keys[i], values[i]);
        }
    }
}

template<class Strategy>
void BM_Lookup(benchmark::State& state) {
    init_data();
    Strategy strategy;
    for (int i = 0; i < N; i++) 
        strategy.insert_test(keys[i], values[i]);
    for (auto _ : state) {
        strategy.lookup_test();
    }
}

template<class Strategy>
void BM_Delete(benchmark::State& state) {
    init_data();
    Strategy strategy;
    for (int i = 0; i < N; i++) 
        strategy.insert_test(keys[i], values[i]);
    for (auto _ : state) {
        strategy.delete_test();
    }
}

// 注册benchmark测试
BENCHMARK(BM_Insert_MaxInsertTime<StdHashMapStrategy>)->Name("STL_Insert_MaxInsertTime")->Unit(benchmark::kMillisecond);
BENCHMARK(BM_Insert_MaxInsertTime<AbslHashMapStrategy>)->Name("Abseil_Insert_MaxInsertTime")->Unit(benchmark::kMillisecond);

BENCHMARK(BM_Insert<StdHashMapStrategy>)->Name("STL_Insert")->Unit(benchmark::kMillisecond);
BENCHMARK(BM_Insert<AbslHashMapStrategy>)->Name("Abseil_Insert")->Unit(benchmark::kMillisecond);

BENCHMARK(BM_Lookup<StdHashMapStrategy>)->Name("STL_Lookup")->Unit(benchmark::kMillisecond);
BENCHMARK(BM_Lookup<AbslHashMapStrategy>)->Name("Abseil_Lookup")->Unit(benchmark::kMillisecond);

BENCHMARK(BM_Delete<StdHashMapStrategy>)->Name("STL_Delete")->Unit(benchmark::kMillisecond);
BENCHMARK(BM_Delete<AbslHashMapStrategy>)->Name("Abseil_Delete")->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();