#include <memory>
#include <iostream>

class HeapOnly {
public:
    static std::unique_ptr<HeapOnly> create() {
        return std::make_unique<HeapOnly>();
    }
    
private:
    // 允许嵌套类访问构造函数
    friend struct std::default_delete<HeapOnly>;
    // HeapOnly() = default;
    ~HeapOnly() { std::cout << "HeapOnly destructor" << std::endl; }
};

class StackOnly {
public:
    StackOnly() = default;
    ~StackOnly() { std::cout << "StackOnly destructor" << std::endl; }

    // 禁止堆上创建对象
    void* operator new(size_t) = delete;
    // 禁止堆上创建数组
    void* operator new[](size_t) = delete;
};

int main() {
    // 堆上创建对象
    auto obj1_heap = HeapOnly::create();
    // 栈上创建对象
    // HeapOnly obj1_stack; // 编译错误，HeapOnly的析构函数是私有的
    
    // 堆上创建对象
    // auto obj2_heap = new StackOnly(); // 编译错误，StackOnly的new运算符被禁用
    // 栈上创建对象
    StackOnly obj1_stack;

    atexit([]() {
        std::cout << "atexit" << std::endl;
    });

    return 0;
}
