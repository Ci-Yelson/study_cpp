#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

std::queue<int> dataQueue;
std::mutex mtx;
std::condition_variable condVar;

void producer() {
    for (int i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // 模拟生产延迟
        std::lock_guard<std::mutex> lock(mtx);
        dataQueue.push(i);
        std::cout << "Produced: " << i << std::endl;
        condVar.notify_one();  // 通知消费者
    }
}

void consumer() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);
        condVar.wait(lock, [] { return !dataQueue.empty(); });  // 等待条件
        int data = dataQueue.front();
        dataQueue.pop();
        lock.unlock();  // 解锁以允许生产者继续工作
        std::cout << "Consumed: " << data << std::endl;
        if (data == 9) break;  // 结束条件
    }
}

int main() {
    std::thread prodThread(producer);
    std::thread consThread(consumer);

    prodThread.join();
    consThread.join();

    return 0;
}
