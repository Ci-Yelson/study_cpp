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

int num_threads = 10;

// join-1
Semaphore sem_join[16];
void worker_init(int T)
{
    num_threads = T;
    for (int i = 0; i < num_threads; i++) {
        sem_join[i].init(0);
    }
}
void worker_done(int tid)
{
    sem_join[tid].V();
}
void worker_join()
{
    for (int i = 0; i < num_threads; i++) {
        sem_join[i].P();
    }
}

// join-2
// Semaphore sem_join;
// void worker_init(int T)
// {
//     num_threads = T;
//     sem_join.init(0);
// }
// void worker_done(int tid)
// {
//     sem_join.V();
// }
// void worker_join()
// {
//     for (int i = 0; i < num_threads; i++) {
//         sem_join.P();
//     }
// }


void T_worker(int tid)
{
    std::cout << "Thread " << tid << " Start" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Thread " << tid << " End" << std::endl;
    worker_done(tid);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <num_threads>" << std::endl;
        return 1;
    }

    num_threads = std::stoi(argv[1]);
    worker_init(num_threads);
    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back(T_worker, i);
    }
    worker_join();
    for (auto &thread : threads) {
        thread.join();
    }
    return 0;
}
