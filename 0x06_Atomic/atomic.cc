#include <atomic>
#include <iostream>
#include <vector>

int main() {
    std::vector<int> data(100);

    std::atomic<int> cnt;
    cnt.store(0);
    for (int i = 0; i < 100; i++) {
        int index = cnt.fetch_add(1);
        data[index] = i; 
    }

    for (int i = 0; i < 100; i++)
        std::cout << data[i] << " ";
    std::cout << std::endl;

}