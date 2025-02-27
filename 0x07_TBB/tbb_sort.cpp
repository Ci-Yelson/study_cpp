#include <vector>
#include <tbb/tbb.h>
#include <benchmark/benchmark.h>

const int N = 1<<24;
const int P = 1<<10;
std::vector<int> a(N);

void BM_sort_seq_std(benchmark::State& state) {
    std::srand(std::time(nullptr));
    std::generate(a.begin(), a.end(), std::rand);
    for (auto _ : state) {
        std::sort(a.begin(), a.end());
    }
}
BENCHMARK(BM_sort_seq_std)->Unit(benchmark::kMillisecond);

// ========================== Qsort ==========================

void BM_sort_qsort_seq_homebrew(benchmark::State& state) {
        std::srand(std::time(nullptr));
    std::generate(a.begin(), a.end(), std::rand);
    std::function<void(int, int)> qsort;
    qsort = [&](int l, int r) {
        if (l >= r) return;
        int i = l - 1, j = r + 1;
        // int x = a[l + rand() % (r - l + 1)];
        int x = a[(l + r) >> 1];
        while (i < j) {
            do i++; while (a[i] < x);
            do j--; while (a[j] > x);
            if (i < j) std::swap(a[i], a[j]);
        }
        qsort(l, j), qsort(j+1, r);
    };
    for (auto _ : state) {
        qsort(0, N-1);
    }
}
BENCHMARK(BM_sort_qsort_seq_homebrew)->Unit(benchmark::kMillisecond);

void BM_sort_qsort_seq_homebrew_cutoff1000(benchmark::State& state) {
        std::srand(std::time(nullptr));
    std::generate(a.begin(), a.end(), std::rand);
    std::function<void(int, int)> qsort;
    qsort = [&](int l, int r) {
        if (r - l <= 1000) {
            std::sort(a.begin() + l, a.begin() + r + 1);
            return ;
        }
        if (l >= r) return;
        int i = l - 1, j = r + 1;
        // int x = a[l + rand() % (r - l + 1)];
        int x = a[(l + r) >> 1];
        while (i < j) {
            do i++; while (a[i] < x);
            do j--; while (a[j] > x);
            if (i < j) std::swap(a[i], a[j]);
        }
        qsort(l, j), qsort(j+1, r);
    };
    for (auto _ : state) {
        qsort(0, N-1);
    }
}
BENCHMARK(BM_sort_qsort_seq_homebrew_cutoff1000)->Unit(benchmark::kMillisecond);

void BM_sort_qsort_tbb_homebrew(benchmark::State& state) {
    std::srand(std::time(nullptr));
    std::generate(a.begin(), a.end(), std::rand);
    std::function<void(int, int)> qsort;
    qsort = [&](int l, int r) {
        if (l >= r) return;
        int i = l - 1, j = r + 1;
        // int x = a[l + rand() % (r - l + 1)];
        int x = a[(l + r) >> 1];
        while (i < j) {
            do i++; while (a[i] < x);
            do j--; while (a[j] > x);
            if (i < j) std::swap(a[i], a[j]);
        }
        // qsort(l, j), qsort(j+1, r);
        tbb::parallel_invoke([&] { qsort(l, j); }, [&] { qsort(j+1, r); });
    };
    for (auto _ : state) {
        qsort(0, N-1);
    }
}
BENCHMARK(BM_sort_qsort_tbb_homebrew)->Unit(benchmark::kMillisecond);

void BM_sort_qsort_tbb_homebrew_cutoff1000(benchmark::State& state) {
    std::srand(std::time(nullptr));
    std::generate(a.begin(), a.end(), std::rand);
    std::function<void(int, int)> qsort;
    qsort = [&](int l, int r) {
        if (r - l <= 1000) {
            std::sort(a.begin() + l, a.begin() + r + 1);
            return ;
        }
        if (l >= r) return;
        int i = l - 1, j = r + 1;
        // int x = a[l + rand() % (r - l + 1)];
        int x = a[(l + r) >> 1];
        while (i < j) {
            do i++; while (a[i] < x);
            do j--; while (a[j] > x);
            if (i < j) std::swap(a[i], a[j]);
        }
        // qsort(l, j), qsort(j+1, r);
        tbb::parallel_invoke([&] { qsort(l, j); }, [&] { qsort(j+1, r); });
    };
    for (auto _ : state) {
        qsort(0, N-1);
    }
}
BENCHMARK(BM_sort_qsort_tbb_homebrew_cutoff1000)->Unit(benchmark::kMillisecond);

// ========================== Msort ==========================

void BM_sort_msort_seq_homebrew(benchmark::State& state) {
    std::srand(std::time(nullptr));
    std::generate(a.begin(), a.end(), std::rand);
    std::vector<int> b(N);
    std::function<void(int, int)> msort;
    msort = [&](int l, int r) {
        if (l >= r) return;
        int mid = (l + r) >> 1;
        msort(l, mid), msort(mid+1, r);

        int k = 0, i = l, j = mid+1;
        while (i <= mid && j <= r)
            b[k++] = (a[i] < a[j]) ? a[i++] : a[j++];
        while (i <= mid) b[k++] = a[i++];
        while (j <= r) b[k++] = a[j++];

        for (int i = l; i <= r; i++) a[i] = b[i-l];
    };
    for (auto _ : state) {
        msort(0, N-1);
    }
}
BENCHMARK(BM_sort_msort_seq_homebrew)->Unit(benchmark::kMillisecond);

void BM_sort_msort_seq_homebrew_cutoff1000(benchmark::State& state) {
    std::srand(std::time(nullptr));
    std::generate(a.begin(), a.end(), std::rand);
    std::vector<int> b(N);
    std::function<void(int, int)> msort;
    msort = [&](int l, int r) {
        if (r - l <= 1000) {
            std::sort(a.begin() + l, a.begin() + r + 1);
            return ;
        }
        if (l >= r) return;
        int mid = (l + r) >> 1;
        msort(l, mid), msort(mid+1, r);

        int k = 0, i = l, j = mid+1;
        while (i <= mid && j <= r)
            b[k++] = (a[i] < a[j]) ? a[i++] : a[j++];
        while (i <= mid) b[k++] = a[i++];
        while (j <= r) b[k++] = a[j++];

        for (int i = l; i <= r; i++) a[i] = b[i-l];
    };
    for (auto _ : state) {
        msort(0, N-1);
    }
}
BENCHMARK(BM_sort_msort_seq_homebrew_cutoff1000)->Unit(benchmark::kMillisecond);


void BM_sort_msort_tbb_homebrew(benchmark::State& state) {
    std::srand(std::time(nullptr));
    std::generate(a.begin(), a.end(), std::rand);
    std::vector<int> b(N);
    std::function<void(int, int)> msort;
    msort = [&](int l, int r) {
        if (l >= r) return;
        int mid = (l + r) >> 1;
        // msort(l, mid), msort(mid+1, r);
        tbb::parallel_invoke([&] { msort(l, mid); }, [&] { msort(mid+1, r); });

        int k = 0, i = l, j = mid+1;
        while (i <= mid && j <= r)
            b[k++] = (a[i] < a[j]) ? a[i++] : a[j++];
        while (i <= mid) b[k++] = a[i++];
        while (j <= r) b[k++] = a[j++];

        for (int i = l; i <= r; i++) a[i] = b[i-l];
    };
    for (auto _ : state) {
        msort(0, N-1);
    }
}
BENCHMARK(BM_sort_msort_tbb_homebrew)->Unit(benchmark::kMillisecond);

void BM_sort_msort_tbb_homebrew_cutoff1000(benchmark::State& state) {
    std::srand(std::time(nullptr));
    std::generate(a.begin(), a.end(), std::rand);
    std::vector<int> b(N);
    std::function<void(int, int)> msort;
    msort = [&](int l, int r) {
        if (r - l <= 1000) {
            std::sort(a.begin() + l, a.begin() + r + 1);
            return ;
        }
        if (l >= r) return;
        int mid = (l + r) >> 1;
        // msort(l, mid), msort(mid+1, r);
        tbb::parallel_invoke([&] { msort(l, mid); }, [&] { msort(mid+1, r); });

        int k = 0, i = l, j = mid+1;
        while (i <= mid && j <= r)
            b[k++] = (a[i] < a[j]) ? a[i++] : a[j++];
        while (i <= mid) b[k++] = a[i++];
        while (j <= r) b[k++] = a[j++];

        for (int i = l; i <= r; i++) a[i] = b[i-l];
    };
    for (auto _ : state) {
        msort(0, N-1);
    }
}
BENCHMARK(BM_sort_msort_tbb_homebrew_cutoff1000)->Unit(benchmark::kMillisecond);
    
// ========================== TBB ==========================

void BM_sort_tbb(benchmark::State& state) {
    std::srand(std::time(nullptr));
    std::generate(a.begin(), a.end(), std::rand);
    for (auto _ : state) {
        tbb::parallel_sort(a.begin(), a.end());
    }
}
BENCHMARK(BM_sort_tbb)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();