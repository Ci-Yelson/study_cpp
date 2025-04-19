#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>

std::mutex mtx;
std::condition_variable cv;

int depth = 0;
int max_depth = 10;
int num_threads = 10;

void T_producer() {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return depth < max_depth; });
        // 1. 等待条件为 true 时，若未持有锁会被放入等待队列中，若持有锁则继续执行；
        // 2. 等待条件为 false 时，会挂起，若持有锁则释放锁；
        
        std::cout << "(";
        depth++;
        
        cv.notify_all(); 
        // 1. 只唤醒那些执行了 cv.wait() 且等待条件为 false 的线程；
        // 2. 执行了 cv.wait() 且被唤醒后重新检查条件为 true 的线程会被放入等待队列中；
        // 3. 执行了 cv.wait() 且被唤醒后重新检查条件为 false 的线程会挂起；
        lock.unlock();
        // 1. 释放锁；
        // 2. 操作系统会从等待队列中选择一个线程（即上面的 cv.wait() 且等待条件为 true 的线程）唤醒
    }
}

void T_consumer() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] { return depth > 0; });
        
        std::cout << ")";
        depth--;
        if (depth == 0) std::cout << std::endl;

        cv.notify_all(); // 只唤醒那些执行了 cv.wait() 且等待条件为 false 的线程
        lock.unlock();
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <max_depth> <num_threads>" << std::endl;
        return 1;
    }

    max_depth = std::stoi(argv[1]);
    num_threads = std::stoi(argv[2]);

    std::vector<std::thread> threads;
    threads.reserve(num_threads * 2);
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(T_producer);
    }
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(T_consumer);
    }

    for (auto &thread : threads) {
        thread.join();
    }
}