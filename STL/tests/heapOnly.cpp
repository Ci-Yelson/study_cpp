#include <iostream>
#include <memory>

class HeapOnly
{
public:
    static std::unique_ptr<HeapOnly> create() { return std::make_unique<HeapOnly>(); }

private:
    // 允许嵌套类访问构造函数
    friend struct std::default_delete<HeapOnly>;
    // HeapOnly() = default;
    ~HeapOnly() { std::cout << "HeapOnly destructor" << std::endl; }
};

class StackOnly
{
public:
    StackOnly() = default;
    ~StackOnly() { std::cout << "StackOnly destructor" << std::endl; }

    // 禁止堆上创建对象
    // void* operator new(size_t) = delete;
    // 禁止堆上创建数组
    void* operator new[](size_t) = delete;

    // 当我们使用 = delete 删除某个重载版本时，它会影响所有相关的重载版本
    // C++ 的重载解析规则：如果基类中的某个重载版本被删除，那么派生类中对应的所有重载版本也会被删除。

    // 显式声明 placement new 版本，从而支持 placement new 在栈上创建对象
    // void* operator new(size_t, void* p) { return p; }
    // 显式声明数组 placement new 版本
    void* operator new[](size_t, void* p) { return p; }
};

// new MyClass(); 调用过程
// 1. 计算需要分配的内存大小
// size_t size = sizeof(MyClass);  // 编译器自动计算
// 2. 调用 operator new 分配内存
// void* raw_mem = operator new(size);
// 3. 在分配的内存上调用构造函数
// MyClass* ptr = static_cast<MyClass*>(raw_mem);
// ptr->MyClass::MyClass();  // 调用构造函数

int main()
{
    // 堆上创建对象
    auto obj1_heap = HeapOnly::create();
    // 栈上创建对象
    // HeapOnly obj1_stack; // 编译错误，HeapOnly的析构函数是私有的

    // 堆上创建对象
    auto* obj2_heap = new StackOnly(); // 编译错误，StackOnly的new运算符被禁用
    // 栈上创建对象
    StackOnly obj1_stack;

    // 在栈上new一个StackOnly对象
    {
        char       obj_buffer[sizeof(StackOnly)];
        StackOnly* obj_stack = new (&obj_buffer[0]) StackOnly();
        obj_stack->~StackOnly();
    }

    atexit([]() { std::cout << "atexit" << std::endl; });

    return 0;
}
