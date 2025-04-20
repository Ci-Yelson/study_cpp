#include <iostream>
#include <thread>
#include <vector>
#include <semaphore>

using namespace std::literals;

std::counting_semaphore<100> sem_empty(0);
std::counting_semaphore<100> sem_full(0);

void T_producer() {
    while (true) {
        sem_empty.acquire();
        std::this_thread::sleep_for(100ms);
        std::cout.put('(').flush();
        sem_full.release();
    }
}

void T_consumer() {
    while (true) {
        sem_full.acquire();
        std::cout.put(')').flush();
        sem_empty.release();
    }
}

int max_depth = 10;
int num_threads = 10;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <max_depth> <num_threads>" << std::endl;
        return 1;
    }

    max_depth = std::stoi(argv[1]);
    num_threads = std::stoi(argv[2]);
    
    sem_empty.release(max_depth);

    std::vector<std::jthread> pool;
    for (int i = 0; i < num_threads; ++i) {
        pool.push_back(std::jthread(T_producer));
        pool.push_back(std::jthread(T_consumer));
    }
}