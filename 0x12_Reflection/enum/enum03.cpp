#include <iostream>

// 定义枚举
#define COLOR_ENUM(x) x,
enum Color {
    #include "enum03_color.def"
};
#undef COLOR_ENUM

// 生成字符串转换函数
#define COLOR_ENUM(x) case Color::x: return #x;
std::string_view Color_to_string(Color value) {
    switch(value) {
        #include "enum03_color.def"
        default: return "";
    }
}
#undef COLOR_ENUM

int main()
{
    std::cout << Color_to_string(Color::RED) << std::endl;
    std::cout << Color_to_string(Color::GREEN) << std::endl;
    std::cout << Color_to_string(Color::BLUE) << std::endl;
    return 0;
}