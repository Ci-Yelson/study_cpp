#include <iostream>
#include <chrono>

template<typename Func, typename... Args>
auto time_function(Func&& func, Args&&... args) {
    auto start = std::chrono::high_resolution_clock::now();
    auto result = std::forward<Func>(func)(std::forward<Args>(args)...);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Time: " 
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << "ms\n";
    return result;
}

// 使用示例
int sum(int a, int b) { return a + b; }

int main() {
    time_function(sum, 1, 2);
    return 0;
}