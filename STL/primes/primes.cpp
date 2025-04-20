#include <bits/stdc++.h>
#include <benchmark/benchmark.h>
#include <numeric>

using namespace std;

const int N = 1e8;
bool is_prime_c[N + 1];
vector<bool> is_prime(N + 1);

class PrimeStrategy {
public:
    virtual vector<int> operator()() = 0;
};

class DefaultPrimes_C : public PrimeStrategy {
public:
    vector<int> operator()() override {
        vector<int> primes;
        // fill(is_prime_c, is_prime_c + N + 1, true);
        is_prime_c[0] = is_prime_c[1] = false;
        for (int i = 2; i <= N; ++i) {
            if (is_prime_c[i]) {
                primes.push_back(i);
                // 枚举 [i*2, i*3, ...]
                for (int j = 2 * i; j <= N; j += i) is_prime_c[j] = false;
            }
        }
        return primes;
    }
};

class DefaultPrimes : public PrimeStrategy {
public:
    vector<int> operator()() override {
        vector<int> primes;
        // fill(is_prime.begin(), is_prime.end(), true);
        is_prime[0] = is_prime[1] = false;
        for (int i = 2; i <= N; ++i) {
            if (is_prime[i]) {
                primes.push_back(i);
                // 枚举 [i*2, i*3, ...]
                for (int j = 2 * i; j <= N; j += i) is_prime[j] = false;
            }
        }
        return primes;
    }
};

class EratoPrimes : public PrimeStrategy {
public:
    vector<int> operator()() override {
        // fill(is_prime.begin(), is_prime.end(), true);
        is_prime[0] = is_prime[1] = false;
        for (int i = 2; i * i <= N; ++i) {
            if (is_prime[i]) { 
                for (int j = i * i; j <= N; j += i) is_prime[j] = false;
            }
        }
        vector<int> primes;
        for (int i = 2; i <= N; i++) 
            if (is_prime[i]) primes.push_back(i);
        return primes;
    }
};

class EulerPrimes : public PrimeStrategy {
public:
    vector<int> operator()() override {
        vector<int> primes;
        // fill(is_prime.begin(), is_prime.end(), true);
        is_prime[0] = is_prime[1] = false;
        for (int i = 2; i <= N; ++i) {
            if (is_prime[i]) primes.push_back(i);
            for (int p : primes) {
                if (1ll * p * i > N) break;
                is_prime[p * i] = false;
                if (i % p == 0) break;
            }
        }
        return primes;
    }
};

template <class PrimeStrategy>
void BM_Generic(benchmark::State& state) {
    // fill(is_prime_c, is_prime_c + N + 1, true);
    // fill(is_prime.begin(), is_prime.end(), true);
    // vector<int> bs_primes = DefaultPrimes()();
    // int err_cnt = 0;
    // auto check_primes = [&](const vector<int>& primes) {
    //     for (int i = 0; i < primes.size(); ++i) {
    //         if (bs_primes[i] != primes[i]) {
    //             cout << "Error at " << i << endl;
    //             err_cnt++;
    //         }
    //     }
    // };
    // static int flag = 1;
    // if (flag)
    // {
    //     cout << std::format("Primes size: {}", bs_primes.size()) << endl;
    //     for (int i = 0; i < 20; ++i) cout << bs_primes[i] << " ";
    //     cout << endl;
    //     flag = 0;
    // }

    fill(is_prime_c, is_prime_c + N + 1, true);
    fill(is_prime.begin(), is_prime.end(), true);

    PrimeStrategy strategy;
    for (auto _ : state) {
        auto primes = strategy();
        benchmark::DoNotOptimize(primes);
        // check_primes(primes);
    }
    // state.SetLabel(std::format("Error count: {}", err_cnt));
}


BENCHMARK(BM_Generic<DefaultPrimes_C>)->Name("DefaultPrimes_C")->Unit(benchmark::kMillisecond)->Iterations(1);
BENCHMARK(BM_Generic<DefaultPrimes>)->Name("DefaultPrimes")->Unit(benchmark::kMillisecond)->Iterations(1);
BENCHMARK(BM_Generic<EratoPrimes>)->Name("EratoPrimes")->Unit(benchmark::kMillisecond)->Iterations(1);
BENCHMARK(BM_Generic<EulerPrimes>)->Name("EulerPrimes")->Unit(benchmark::kMillisecond)->Iterations(1);
BENCHMARK_MAIN();

