#include <atomic>
#include <iostream>
#include <mutex>

// ========================== Singleton01 【单线程】【指针不能正确释放】 ==========================
// 懒汉式单例
class Singleton01 {
public:
    static Singleton01* GetInstance() {
        if (m_instance == nullptr) {
            m_instance = new Singleton01();
        }
        return m_instance;
    }

private:
    Singleton01() { std::cout << "Singleton01 constructor" << std::endl; }
    // 不能在析构函数中直接释放m_instance，因为会导致递归调用
    // 当调用delete m_instance时，会触发析构函数，而析构函数中再次delete会导致无限递归或者重复释放。
    ~Singleton01() { std::cout << "Singleton01 destructor" << std::endl; }

    // 禁止拷贝构造和赋值
    Singleton01(const Singleton01&) = delete;
    Singleton01& operator=(const Singleton01&) = delete;
    // 禁止移动构造和移动赋值
    Singleton01(Singleton01&&) = delete;
    Singleton01& operator=(Singleton01&&) = delete;

    static Singleton01* m_instance;
};
// 初始化静态成员变量
Singleton01* Singleton01::m_instance = nullptr;
// Singleton01 存在问题：
// 1. 不支持多线程
// 2. m_instance在程序结束后不会被自动释放，需要手动释放

// ========================== Singleton02 【单线程】【指针能正确释放】 ==========================
// 懒汉式单例
class Singleton02 {
public:
    static Singleton02* GetInstance() {
        if (m_instance == nullptr) {
            m_instance = new Singleton02();
            // 当程序正常退出时（通过main返回或调用exit），C++运行时会自动调用已注册的函数
            atexit(DestroyInstance);
        }
        return m_instance;
    }
private:
    static void DestroyInstance() {
        if (m_instance != nullptr) {
            delete m_instance;
            m_instance = nullptr;
        }
    }

private:
    Singleton02() { std::cout << "Singleton02 constructor" << std::endl; }
    // 不能在析构函数中直接释放m_instance，因为会导致递归调用
    // 当调用delete m_instance时，会触发析构函数，而析构函数中再次delete会导致无限递归或者重复释放。
    ~Singleton02() { std::cout << "Singleton02 destructor" << std::endl; }

    // 禁止拷贝构造和赋值
    Singleton02(const Singleton02&) = delete;
    Singleton02& operator=(const Singleton02&) = delete;
    // 禁止移动构造和移动赋值
    Singleton02(Singleton02&&) = delete;
    Singleton02& operator=(Singleton02&&) = delete;

    static Singleton02* m_instance;
};
// 初始化静态成员变量
Singleton02* Singleton02::m_instance = nullptr;
// Singleton02:
// - 使用atexit注册一个函数，在程序退出时自动释放m_instance。
// - 但是，当程序异常退出时（如调用abort()、触发段错误、或收到未处理的信号等），atexit注册的销毁函数不会被执行，m_instance不会被释放。

// ========================== Singleton03 【多线程】【双检查锁，内存序问题】 ==========================
// 懒汉式单例
// 线程安全
// 使用双重检查锁定（Double-Checked Locking），但存在内存序问题
class Singleton03 {
public:
    static Singleton03* GetInstance() {
        // Version 1
        // std::lock_guard<std::mutex> lock(m_mutex);
        // if (m_instance == nullptr) {
        //     m_instance = new Singleton03();
        //     atexit(DestroyInstance);
        // }
        // Version 2 - 双重检查锁定
        if (m_instance == nullptr) {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_instance == nullptr) {
                m_instance = new Singleton03();
                // 此处仍有内存序问题，因为new操作不是原子操作
                // new 操作：
                // 1. 分配内存空间
                // 2. 调用构造函数初始化对象
                // 3. 将地址赋值给指针
                // 编译器/CPU可能重排步骤为：1→3→2，导致其他线程访问到未初始化的对象
                // 解决方案：使用原子操作或内存屏障
                atexit(DestroyInstance);
            }
        }
        return m_instance;
    }
private:    
    static void DestroyInstance() {
        if (m_instance != nullptr) {
            delete m_instance;
            m_instance = nullptr;
        }
    }
    Singleton03() { std::cout << "Singleton03 constructor" << std::endl; }
    ~Singleton03() { std::cout << "Singleton03 destructor" << std::endl; }
    // 禁止拷贝构造和赋值
    Singleton03(const Singleton03&) = delete;
    Singleton03& operator=(const Singleton03&) = delete;
    // 禁止移动构造和移动赋值
    Singleton03(Singleton03&&) = delete;
    Singleton03& operator=(Singleton03&&) = delete;

    static Singleton03* m_instance;
    static std::mutex m_mutex;
};
// 初始化静态成员变量
Singleton03* Singleton03::m_instance = nullptr;
std::mutex Singleton03::m_mutex;

// ========================== Singleton04 【多线程】【双检查锁 + 原子操作（可见性） + 内存屏障】 ==========================
// 懒汉式单例
// 线程安全
// 双重检查锁定（Double-Checked Locking） + 原子操作（可见性） + 内存屏障
class Singleton04 {
public:
    static Singleton04* GetInstance() {
        // 双重检查锁定 + 原子操作（可见性） + 内存屏障
        Singleton04* instance = m_instance.load(); 
        std::atomic_thread_fence(std::memory_order_acq_rel); // 暂时使用保守的内存屏障
        if (instance == nullptr) {
            std::lock_guard<std::mutex> lock(m_mutex);
            instance = m_instance.load(); // 在加锁后再次检查
            if (instance == nullptr) {
                instance = new Singleton04();
                std::atomic_thread_fence(std::memory_order_acq_rel); // 暂时使用保守的内存屏障
                m_instance.store(instance);
                atexit(DestroyInstance);
            }
        }
        return m_instance;
    }
private:
    static void DestroyInstance() {
        Singleton04* instance = m_instance.load(std::memory_order_acq_rel); // 暂时使用保守的内存序
        if (instance != nullptr) {
            delete instance;
        }
    }
    Singleton04() { std::cout << "Singleton04 constructor" << std::endl; }
    ~Singleton04() { std::cout << "Singleton04 destructor" << std::endl; }
    // 禁止拷贝构造和赋值
    Singleton04(const Singleton04&) = delete;
    Singleton04& operator=(const Singleton04&) = delete;
    // 禁止移动构造和移动赋值
    Singleton04(Singleton04&&) = delete;
    Singleton04& operator=(Singleton04&&) = delete;

    // 使用原子操作，目的是为了在多线程环境下，对m_instance的写操作能被其他线程立即看到
    static std::atomic<Singleton04*> m_instance; 
    static std::mutex m_mutex;
};
// 初始化静态成员变量
std::atomic<Singleton04*> Singleton04::m_instance;
std::mutex Singleton04::m_mutex;

// ========================== Singleton05 【多线程】【静态局部变量】【现代C++写法】 ==========================
// 饿汉式单例
// 线程安全
class Singleton05 {
public:
    static Singleton05* GetInstance() {
        // 1. 延迟加载：真正按需创建（首次调用时构造），而非传统饿汉式的程序启动即初始化
        // 2. 线程安全：C++11标准保证静态局部变量的初始化是线程安全的，无需额外加锁
        // 3. 自动释放：静态变量在程序退出时自动析构，无需手动delete或注册atexit
        // 4. 内存序安全：编译器保证静态变量初始化的原子性和可见性，无DCLP（双重检查锁定）问题
        static Singleton05 instance;
        return &instance;
    }
private:
    Singleton05() { std::cout << "Singleton05 constructor" << std::endl; }
    ~Singleton05() { std::cout << "Singleton05 destructor" << std::endl; }
    // 禁止拷贝构造和赋值
    Singleton05(const Singleton05&) = delete;
    Singleton05& operator=(const Singleton05&) = delete;
    // 禁止移动构造和移动赋值
    Singleton05(Singleton05&&) = delete;
    Singleton05& operator=(Singleton05&&) = delete;
};


// ========================== Singleton Template 【CRTP模式】 ==========================
template <typename T>
class Singleton {
public:
    static T* GetInstance() {
        static T instance; // 线程安全的延迟初始化（C++11标准保证）
        return &instance;
    }
protected: // 父类构造函数protected：允许子类继承但禁止外部实例化
    Singleton() {}
    virtual ~Singleton() {}
private:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;
};

// CRTP模式（奇异递归模板模式）
// - 子类继承时将自己作为模板参数：DesignPattern : public Singleton<DesignPattern>
// - 实现编译期多态，父类可以访问子类成员
// 访问控制
// - 父类构造函数protected：允许子类继承但禁止外部实例化
// - 子类构造函数private + 友元声明：只允许Singleton模板创建实例
class CRTPDesignPattern : public Singleton<CRTPDesignPattern> {
    // 关键友元声明 - 允许Singleton类访问DesignPattern的私有构造函数
    friend class Singleton<CRTPDesignPattern>;
private:
    CRTPDesignPattern() { std::cout << "CRTPDesignPattern constructor" << std::endl; }
    ~CRTPDesignPattern() { std::cout << "CRTPDesignPattern destructor" << std::endl; }
};


int main() {
    return 0;
}