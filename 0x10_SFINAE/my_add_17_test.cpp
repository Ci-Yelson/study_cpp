#include "my_add_17.hpp"
#include <iostream>
#include <vector>

struct A
{
    int val = 0;
    A   operator+(const A& other) const { return A {val + other.val}; }
};

template<class T>
struct TA
{
    T  val = 0;
    TA operator+(const TA& other) const { return TA {val + other.val}; }
    template<class U>
    auto operator+(const TA<U>& other) const
    {
        return TA<decltype(val + other.val)> {val + other.val};
    }
};

int main()
{
    // 基础类型加法
    {
        int    a = 1;
        double b = 3.7;
        std::cout << my_add(a, b) << std::endl; // 4.7
    }
    {
        TA<int>    a {1};
        TA<double> b {3.7};
        auto       c = my_add(a, b).val;
        std::cout << c << std::endl; // 4.7
    }
    // 容器类型加法
    {
        std::vector<int>    a = {1, 2, 3, 4};
        std::vector<double> b = {0.7, 0.7, 0.7};
        auto                c = my_add(a, b);
        for (auto v : c)
            std::cout << v << " ";
        std::cout << "\n";
    }
    {
        std::vector<int> a = {int {1}, int {2}, int {3}, int {4}};
        std::vector<int> b = {int {3}, int {3}, int {3}};
        auto             c = my_add(a, b);
        for (auto v : c)
            std::cout << v << " ";
        std::cout << "\n";
    }
}