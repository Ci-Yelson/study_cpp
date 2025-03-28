#include <iostream>

class A {
public:
    A() {
        std::cout << "A()" << std::endl;
    }
    ~A() {
        std::cout << "~A()" << std::endl;
    }

    void func() {
        std::cout << "func() - Success" << std::endl;
    }

    virtual void virtual_func() {
        std::cout << "virtual_func() - Success" << std::endl;
    }

    static void static_func() {
        std::cout << "static_func() - Success" << std::endl;
    }

    int val = 10;
};


int main() {
    A* a = nullptr;
    a->func(); // 可以执行
    a->static_func(); // 可以执行
    // a->virtual_func(); // segmentation fault
    // std::cout << a->val << std::endl; // segmentation fault
    delete a;
    return 0;
}
