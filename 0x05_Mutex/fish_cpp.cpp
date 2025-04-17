#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>

#define LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

const char roles[] = ".<<<<<>>>>___ABCD";

enum {A = 1, B, C, D, E, F,};
struct Rule {
    int from, ch, to;
} rules[] = {
    // {A, '<', B},
    // {B, '>', C},
    // {C, '<', D},
    // {A, '>', E},
    // {E, '<', F},
    // {F, '>', D},
    // {D, '_', A},
    {A, 'A', B},
    {B, 'B', C},
    {C, 'C', D},
    {D, 'D', E},
    {E, '_', A},
};

int curr_state = A;
int quota = 1;
std::mutex mtx;
std::condition_variable cv;

int next_state(int state, char ch) {
    for (int i = 0; i < LENGTH(rules); i++) {
        if (rules[i].from == state && rules[i].ch == ch) {
            return rules[i].to;
        }
    }
    return 0;
}

void fish_before(char ch) {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [&]() {
        return next_state(curr_state, ch) != 0 && quota > 0;
    });
    quota--;
    lock.unlock();
}

void fish_after(char ch) {
    std::unique_lock<std::mutex> lock(mtx);
    curr_state = next_state(curr_state, ch);
    quota++;
    cv.notify_all();
    lock.unlock();
}

void fish_thread(char ch) {
    while (true) {
        fish_before(ch);
        std::cout << ch;
        if (ch == '_') {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            std::cout << std::endl;
        }
        fish_after(ch);
    }
}

int main() {
    std::vector<std::thread> threads;
    threads.reserve(LENGTH(roles));
    for (int i = 0; i < LENGTH(roles); i++) {
        threads.emplace_back(fish_thread, roles[i]);
    }
    for (auto &thread : threads) {
        thread.join();
    }
}