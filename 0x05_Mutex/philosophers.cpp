#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>

struct Semaphore {
    int value;
    std::mutex mtx;
    std::condition_variable cv;

    void init(int value) {
        this->value = value;
    }

    void P() { // Acquire
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]() { return value > 0; });
        value--;
        lock.unlock();
    }

    void V() { // Release
        std::unique_lock<std::mutex> lock(mtx);
        value++;
        cv.notify_all();
        lock.unlock();
    }
};

const int num_philosophers = 5;
Semaphore sem_table;
Semaphore sem_forks[num_philosophers];

void T_philosopher(int tid) {
    int lhs = (tid + 1) % num_philosophers;
    int rhs = tid;

    while (true) {
        sem_table.P();

        // // printf 是原子的，一次输出完整的格式化字符串
        // printf("+ Fork %d by %d\n", lhs, tid);  // 线程安全
        // // std::cout 默认情况下每个 << 操作都是独立的
        // std::cout << "+ Fork " << lhs << " by " << tid << std::endl;  // 可能被打断

        sem_forks[lhs].P();
        // std::cout << "+ Fork " << lhs << " by " << tid << std::endl;
        printf("+ Fork %d by %d\n", lhs, tid);
        sem_forks[rhs].P();
        // std::cout << "+ Fork " << rhs << " by " << tid << std::endl;
        printf("+ Fork %d by %d\n", rhs, tid);
        
        printf("Philosopher %d is eating\n", tid);
        std::this_thread::sleep_for(std::chrono::seconds(1));
 
        sem_forks[lhs].V();
        // std::cout << "- Fork " << lhs << " by " << tid << std::endl;
        printf("- Fork %d by %d\n", lhs, tid);
        sem_forks[rhs].V();
        // std::cout << "- Fork " << rhs << " by " << tid << std::endl;
        printf("- Fork %d by %d\n", rhs, tid);

        sem_table.V();
    }
}

int main() {
    sem_table.init(num_philosophers - 1);
    for (int i = 0; i < num_philosophers; i++) {
        sem_forks[i].init(1);
    }

    std::vector<std::thread> threads;
    threads.reserve(num_philosophers);
    for (int i = 0; i < num_philosophers; i++) {
        threads.emplace_back(T_philosopher, i);
    }
    for (auto &thread : threads) {
        thread.join();
    }
    return 0;
}