#include <iostream>
#include <future>
#include <thread>
#include <chrono>


int download(std::string url)
{
    std::thread::id thread_id = std::this_thread::get_id();
    for (int i = 0; i < 10; i++)
    {
        std::cout << "Downloading " << url << " (" << i * 10 << "%)" << " on thread " << thread_id << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }
    std::cout << "Download complete: " << url << " on thread " << thread_id << std::endl;
    return 200;
}


void inertact()
{
    std::thread::id thread_id = std::this_thread::get_id();
    std::string input;
    std::cout << "Inertacting...\nPlease input your name on thread " << thread_id << ": \n";
    std::cin >> input;
    std::cout << "Hello, " << input << "!" << " on thread " << thread_id << std::endl;
}

int main()
{
    // === thread + promise + future ===
    // std::promise<int> p;
    // std::thread t([&p] {
    //     p.set_value(download("https://example.com/file1.txt"));
    // });
    // std::future<int> f = p.get_future();

    // === async + future ===
    std::future<int> f = std::async(download, "https://example.com/file1.txt");
    
    inertact();
    int result = f.get();
    std::cout << "Download result: " << result << std::endl;

    // t.join();
}