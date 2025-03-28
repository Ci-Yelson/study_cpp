#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <bitset>

void examine_int_hash() {
    std::hash<int> hasher;
    std::unordered_map<int, int> map;
    map.bucket_count();
    
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

void examine_string_hash() {
    std::hash<std::string> hasher;
    
    std::string test_values[] = {
        "hello",
        "world",
        "test",
        "example",
        "abcdabcdabcdabcdabcdabcdabcdabcd",
        "abcfabcfabcfabcfabcfabcfabcfabcf",
        "\x00\x00\x00\x00\x2d\x18\x7f\x2d",
        "\x2d\x18\x7f\x2d\x00\x00\x00\x00",
    };      

    for (const std::string& val : test_values) {
        size_t hash_val = hasher(val);
        std::cout << "字符串: " << val 
                  << "\n哈希值: " << hash_val 
                  << "\n\n";
    }
}

void generate_collision() {
    const uint32_t magic = 0x2d187f2d;  // 经过验证的魔数
    std::string s1(4, '\x00');
    s1.append(reinterpret_cast<const char*>(&magic), 4);
    
    std::string s2(reinterpret_cast<const char*>(&magic), 4);
    s2.append(4, '\x00');

    std::hash<std::string> hasher;
    std::cout << "Collision check: " 
              << (hasher(s1) == hasher(s2) ? "Success" : "Fail")
              << "\nHash value: " << std::hex << hasher(s1)
              << "\nHash value: " << hasher(s2) << std::dec << "\n";
}

int main() {
    // examine_int_hash();
    examine_string_hash();
    // generate_collision();
    return 0;
}