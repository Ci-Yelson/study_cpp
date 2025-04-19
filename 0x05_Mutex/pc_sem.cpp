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

Semaphore sem_empty;
Semaphore sem_full;

int max_depth = 10;
int num_threads = 10;

void T_producer() {
    while (true) {
        sem_empty.P();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::cout << "(" << std::flush;
        sem_full.V();
    }
}

void T_consumer() {
    while (true) {
        sem_full.P();
        std::cout << ")" << std::flush;
        sem_empty.V();
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <max_depth> <num_threads>" << std::endl;
        return 1;
    }

    max_depth = std::stoi(argv[1]);
    num_threads = std::stoi(argv[2]);

    sem_empty.init(max_depth);
    sem_full.init(0);
    
    std::vector<std::thread> threads;
    threads.reserve(num_threads * 2);
    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back(T_producer);
        threads.emplace_back(T_consumer);
    }

    for (auto &thread : threads) {
        thread.join();
    }
}
