#include <iostream>
#include <unordered_map>
#include <bitset>

void examine_int_hash() {
    std::hash<int> hasher;
    
    // 测试一些特殊值
    int test_values[] = {
        0,
        1,
        -1,
        42,
        -42,
        // INT_MAX,
        // INT_MIN
    };
    
    for (int val : test_values) {
        size_t hash_val = hasher(val);
        std::cout << "整数: " << val 
                  << "\n哈希值: " << hash_val 
                  << "\n二进制: " << std::bitset<64>(hash_val) 
                  << "\n\n";
    }
}

int main() {
    examine_int_hash();
    return 0;
}