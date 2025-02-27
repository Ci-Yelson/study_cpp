#pragma once
#include <atomic>


class SpinMutex {
    std::atomic<bool> m_flag{false};

public:
    bool try_lock() {
        bool old = false;
        // 成功时采用std::memory_order_acquire，失败时采用std::memory_order_relaxed
        if (m_flag.compare_exchange_strong(old, true, std::memory_order_acquire, std::memory_order_relaxed)) 
            // Load Barrier
            return true;
        return false;
    }

    void lock() {
        bool old = false;
        // 成功时采用std::memory_order_acquire，失败时采用std::memory_order_relaxed
        while (!m_flag.compare_exchange_weak(old, true, std::memory_order_acquire, std::memory_order_relaxed)) 
            old = false; // Note: compare_exchange_weak 会修改 old 的值
        // Load Barrier
    }

    void unlock() {
        // Store Barrier
        m_flag.store(false, std::memory_order_release);
    }
};

// SpinMutexWrongInARM 在 X86 架构上不会出问题，但在 ARM 架构上会出错.
class SpinMutexWrongInARM {
    std::atomic<bool> m_flag{false};

public:
    bool try_lock() {
        bool old = false;
        // 成功和失败都采用std::memory_order_relaxed
        if (m_flag.compare_exchange_strong(old, true, std::memory_order_relaxed)) 
            return true;
        return false;
    }

    void lock() {
        bool old = false;
        // 成功和失败都采用std::memory_order_relaxed
        while (!m_flag.compare_exchange_weak(old, true, std::memory_order_relaxed)) 
            old = false; // Note: compare_exchange_weak 会修改 old 的值
    }

    void unlock() {
        m_flag.store(false, std::memory_order_relaxed);
    }
};

class SpinMutexCPP20 {
    std::atomic<bool> m_flag{false};

public:
    bool try_lock() {
        bool old = false;
        // 成功时采用std::memory_order_acquire，失败时采用std::memory_order_relaxed
        if (m_flag.compare_exchange_strong(old, true, std::memory_order_acquire, std::memory_order_relaxed)) 
            // Load Barrier
            return true;
        return false;
    }

    void lock() {
        bool old = false;
#if __cpp_lib_atomic_wait
        int retries = 100;
        do {
            if (m_flag.compare_exchange_weak(old, true, std::memory_order_acquire, std::memory_order_relaxed)) 
                return ;
        } while (--retries);
#endif
        // 成功时采用std::memory_order_acquire，失败时采用std::memory_order_relaxed
        while (!m_flag.compare_exchange_weak(old, true, std::memory_order_acquire, std::memory_order_relaxed)) {
#if __cpp_lib_atomic_wait
            // fast-user-space mutex = futex (linux) SYS_futex
            m_flag.wait(true, std::memory_order_relaxed); // wait 会阻塞，直到 m_flag != true
#endif
            old = false; // Note: compare_exchange_weak 会修改 old 的值
        }
        // Load Barrier
    }

    void unlock() {
        // Store Barrier
        m_flag.store(false, std::memory_order_release);
#if __cpp_lib_atomic_wait
        m_flag.notify_one();
#endif
    }
};