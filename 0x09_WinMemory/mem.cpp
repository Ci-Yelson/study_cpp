#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <chrono>
#include <iostream>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#else
#include <sys/resource.h>
#endif

const int N = int(1e4);
const int M = 6 * N;

class MemTest {
    Eigen::SparseMatrix<double, Eigen::RowMajor> Si;
    Eigen::SparseMatrix<double, Eigen::ColMajor> Si_T;

public:
    MemTest() {
        Si.setZero();
        Si.resize(12, N);
        for (int i = 0; i < 12; i++) {
            Si.insert(i, i) = 7;
        }
        Si_T = Si.transpose();
    }
    
};

size_t GetMemoryUsage() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize / 1024; // 返回以KB为单位的内存使用量
    }
    return 0;
#else
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss; // 返回以KB为单位的内存使用量
#endif
}

int main() {
    std::vector<MemTest> mem_test(M);
    std::cout << "mem_test.size() = " << mem_test.size() << std::endl;
    while (true) {
        std::cout << "Memory usage: " << (GetMemoryUsage() / 1024.0) << " MB" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}