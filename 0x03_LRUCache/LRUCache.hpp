#pragma once

#include <functional>
#include <list>
#include <mutex>
#include <unordered_map>

template <typename Key, typename Val>
class LRUCache {
private:
    int m_capacity;
    // 双向链表 - std::list
    std::list<std::pair<Key, Val*>> m_cache;
    // 没有typename时，编译器会认为::iterator可能是一个静态成员变量而不是类型
    std::unordered_map<Key, typename std::list<std::pair<Key, Val*>>::iterator> m_hashmap;
    
    std::function<void(const Key&, Val*)> m_free_value_func = nullptr;
    static void default_free_value(const Key& key, Val* value)
    {
        delete value;
    }

public:
    explicit LRUCache(int capacity, const std::function<void(const Key&, Val*)>& free_value_func = default_free_value)
        : m_capacity(capacity), m_free_value_func(free_value_func) {}
    Val* get(const Key& key)
    {
        auto it = m_hashmap.find(key);
        if (it == m_hashmap.end()) {
            return nullptr;
        }
        // m_cache.splice(m_cache.begin(), m_cache, it->second);
        auto& node_iter = it->second;
        auto& node_pair = *node_iter;
        m_cache.erase(node_iter);
        m_cache.insert(m_cache.begin(), node_pair);
        Val* value = node_pair.second;
        return value;
    }
    void put(const Key& key, Val* value)
    {
        auto it = m_hashmap.find(key);
        if (it != m_hashmap.end()) {
            // Update the value of the existing key
            auto& node_pair = *it->second;
            node_pair.second = value;
            // Update the position of the existing key in the cache
            m_cache.erase(it->second);
            m_cache.insert(m_cache.begin(), node_pair);
        } else {
            if (m_cache.size() >= m_capacity) {
                // Remove the last element from the cache
                auto& last_node_pair = m_cache.back();
                Key last_key = last_node_pair.first;
                Val* last_value_ptr = last_node_pair.second;
                m_hashmap.erase(last_key);
                m_free_value_func(last_key, last_value_ptr);
                m_cache.pop_back();
            }
            // Insert the new key-value pair into the cache
            m_cache.insert(m_cache.begin(), std::make_pair(key, value));
            m_hashmap[key] = m_cache.begin();
        }
    }
};

// 代理模式+RAII锁管理的 MT_LRUCache
template <typename Key, typename Val>
class MT_LRUCache {
private:
    LRUCache<Key, Val> m_lru_cache;
    std::mutex m_mutex;
public:
    class Accessor {
    private:
        MT_LRUCache& m_that;
        std::unique_lock<std::mutex> m_lock;
    public:
        explicit Accessor(MT_LRUCache& that): m_that(that), m_lock(m_that.m_mutex) {}
        Val* get(const Key& key)
        {
            return m_that.m_lru_cache.get(key);
        }
        void put(const Key& key, Val* value)
        {
            m_that.m_lru_cache.put(key, value);
        }
    };

    Accessor access() { return Accessor{*this}; }
};

// class SafeFileHandle {
//     FILE* fp;
// public:
//     explicit operator bool() const {  // 关键修改
//         return fp != nullptr;
//     }
// };

// SafeFileHandle sfh = open_file();

// // 合法使用场景 ✅
// if (sfh) { ... }                 // 允许（上下文转换）
// bool valid = sfh ? true : false; // 允许（条件运算符）
// for (; sfh; ) { ... }            // 允许（循环条件）

// // 非法使用场景 ❌
// bool status = sfh;        // 错误：需要 static_cast<bool>
// int x = sfh;              // 错误：无法隐式转换
// auto y = sfh + 5;         // 错误：禁止参与运算

// class UniqueID {
//     int id;
// public:
//     explicit UniqueID(int v) : id(v) {}
//     explicit UniqueID(const UniqueID& other) : id(other.id) {}  // 显式拷贝
// };

// UniqueID id1(100);
// UniqueID id2 = id1;   // 错误 ❌
// UniqueID id3(id1);    // 正确 ✅

struct PodTrap {      
    int x = 1;            
    char y{'a'};           
    double z{};         
};             