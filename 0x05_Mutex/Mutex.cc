#include <iostream>
#include <thread>
#include <mutex>
#include <vector>


int main()
{
    std::vector<int> a;
    std::mutex mtx;
    std::thread t1([&] {
        for (int i = 0; i < 100; i++) {
            // 1. mtx.lock() & mtx.unlock()
            // mtx.lock();
            // a.push_back(3);
            // mtx.unlock();

            // 2. std::lock_guard grd(mtx)
            // std::lock_guard grd(mtx);
            // a.push_back(3);

            // 3. std::unique_lock grd(mtx)
            std::unique_lock grd(mtx);
            a.push_back(3);
        }
    });
    std::thread t2([&] {
        for (int i = 0; i < 100; i++) {
            mtx.lock();
            a.push_back(7);
            mtx.unlock();
        }
    });
    t1.join();
    t2.join();
    for (int i : a) {
        std::cout << i << " ";
    }

}