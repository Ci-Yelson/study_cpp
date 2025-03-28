#include <iostream>
#include <string>
#include <format>

using namespace std;

std::string operator""_m(const char* str, size_t size) {
    return "#M_" + std::string(str, size);
}

int main() {
    string str = "Hello, World!"_m;
    cout << str << endl;

    string_view str_view = str;
    cout << str_view << endl;

    str[0] = 'M';
    cout << str_view << endl;

    // string_view(255, 17): Function 'operator[]' which returns const-qualified type 'const_reference' (aka 'const char &') declared here
    // str_view[0] = 'A'; 
    // cout << str << endl;

    str = "ABCDEFG";
    cout << str << endl;
    cout << str_view << endl;

    // {
    //     string str = "1234567"s;
    //     string_view str_view = str;
    //     cout << std::format("1) STR: \t{}\n1) STR_VIEW: \t{}", str, str_view) << endl;
    //     // 1) STR:         1234567
    //     // 1) STR_VIEW:    1234567
    //     str[0] = 'A';
    //     cout << std::format("2) STR: \t{}\n2) STR_VIEW: \t{}", str, str_view) << endl;
    //     // 2) STR:         A234567
    //     // 2) STR_VIEW:    A234567
    //     str = "ABC";
    //     cout << std::format("3) STR: \t{}\n3) STR_VIEW: \t{}", str, str_view) << endl;
    //     // 3) STR:         ABC
    //     // 3) STR_VIEW:    ABC567
    //     // 导致重新分配内存
    //     str = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    //     // 危险！这时访问string_view将导致未定义行为
    //     cout << std::format("4) STR: \t{}\n4) STR_VIEW: \t{}", str, str_view) << endl;
    //     // 4) STR:         ABCDEFGHIJKLMNOPQRSTUVWXYZ
    //     // 4) STR_VIEW:    
    // }

    { // 小字符串优化 [32 bytes = 8 bytes(vptr) + 8 bytes(size) + 16 bytes(union local_buf, capacity)]
        string s0;
        string s1 = "1";
        string s2 = "1234567890";
        string s3 = "12345678901234567890";
        cout << std::format("sizeof s0: \t{}, size: {}\n", sizeof(s0), s0.size());
        cout << std::format("sizeof s1: \t{}, size: {}\n", sizeof(s1), s1.size());
        cout << std::format("sizeof s2: \t{}, size: {}\n", sizeof(s2), s2.size());
        cout << std::format("sizeof s3: \t{}, size: {}\n", sizeof(s3), s3.size());
    }

    return 0;
}