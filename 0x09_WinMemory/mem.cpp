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

// ===================== 压缩列存储 (Compressed Column Storage, CCS) =====================
// 内存结构：
// - values[N]: 非零元素值数组 (sizeof(T) * N)
// - innerIndices[N]: 行索引数组 (sizeof(Index) * N)
// - outerStarts[Cols+1]: 列指针数组 (sizeof(Index) * (Cols+1))

// 总内存 ≈ N*(sizeof(T)+sizeof(Index)) + (Cols+1)*sizeof(Index)
// ======================================================================================

// ===================== 压缩行存储 (Compressed Row Storage, CRS) =====================
// 内存结构：
// - values[N]: 非零元素值数组 (sizeof(T) * N)
// - innerIndices[N]: 列索引数组 (sizeof(Index) * N)
// - outerStarts[Rows+1]: 行指针数组 (sizeof(Index) * (Rows+1))

// 总内存 ≈ N*(sizeof(T)+sizeof(Index)) + (Rows+1)*sizeof(Index)
// ====================================================================================== 

// 指针数组开销：
//  CCS：4(列数+1) 字节
//  CRS：4(行数+1) 字节
// 当 行数 ≫ 列数 时，CCS 更高效
// 当 列数 ≫ 行数 时，CRS 更高效
// 示例计算（1000x100矩阵，NNZ=5000）：
// CCS内存 ≈ 5000*(8+4) + 101*4 ≈ 60,404 字节
// CRS内存 ≈ 5000*(8+4) + 1001*4 ≈ 64,004 字节

// ====================================================================================== 
// Windows测试 - N=3e5时, Memory usage: 4695.63 MB, 提交内存：78.7/79.2 GB; 继续增大N会崩溃.
// Linux测试 - N=2e6时，Memory usage: 14533.4 MB
const int N = int(3e5);
const int MAT_N = int(1e4);

class MemTest {
    Eigen::SparseMatrix<double, Eigen::RowMajor> Si;    // CRS
    // Eigen::SparseMatrix<double, Eigen::ColMajor> Si_T;  // CCS

public:
    MemTest() {
        Si.setZero();
        Si.resize(12, MAT_N);
        for (int i = 0; i < 12; i++) {
            Si.insert(i, i) = 7;
        }
        // Si.makeCompressed(); // 启用makeCompressed()后，不再会出现虚存爆炸的问题.
    }
};


int main() {
    std::vector<MemTest> mem_test(N);
    std::cout << "mem_test.size() = " << mem_test.size() << std::endl;
    while (true) {
        std::cout << "Memory usage: " << (GetMemoryUsage() / 1024.0) << " MB" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}