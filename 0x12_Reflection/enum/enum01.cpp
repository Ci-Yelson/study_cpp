// https://zhuanlan.zhihu.com/p/680412313
// 模板打表实现枚举名获取，实现原理与 magic_enum 一致

// __PRETTY_FUNCTION__ 或 __FUNCSIG__ 获取函数签名
// Trick: 当枚举值没有对应的枚举项时，打印的不是对应的枚举名，而是带有括号的强制转换表达式。
// 递归获取枚举的最大值
// 通过 make_index_sequence 打表获取枚举名数组

#include <array>
#include <iostream>

template<auto T>
void print_fn()
{
#if __GNUC__ || __clang__
    std::cout << __PRETTY_FUNCTION__ << std::endl;
#else
    std::cout << __FUNCSIG__ << std::endl;
#endif
}

enum Color
{
    RED,
    GREEN,
    BLUE
};

enum class ColorClass
{
    RED,
    GREEN,
    BLUE
};

template<auto value>
constexpr auto enum_name()
{
    std::string_view name;
#if __GNUC__ || __clang__
    name              = __PRETTY_FUNCTION__;
    std::size_t start = name.find('=') + 2;
    std::size_t end   = name.size() - 1;
    name              = std::string_view {name.data() + start, end - start};
    start             = name.rfind("::");
#elif _MSC_VER
    name              = __FUNCSIG__;
    std::size_t start = name.find('<') + 1;
    std::size_t end   = name.rfind(">(");
    name              = std::string_view {name.data() + start, end - start};
    start             = name.rfind("::");
#endif
    return start == std::string_view::npos ? name : std::string_view {name.data() + start + 2, name.size() - start - 2};
}

void static_test()
{
    std::cout << "--------------------------------static_test--------------------------------" << std::endl;
    print_fn<1>();
    // gcc and clang => void print_fn() [with auto T = 1]
    // msvc => void __cdecl print_fn<1>(void)

    print_fn<Color::RED>();
    // gcc and clang => void print_fn() [with auto T = RED]
    // msvc => void __cdecl print_fn<RED>(void)

    print_fn<ColorClass::RED>();
    // gcc and clang => void print_fn() [with auto T = ColorClass::RED]
    // msvc => void __cdecl print_fn<ColorClass::RED>(void)

    std::cout << enum_name<Color::RED>() << std::endl;      // RED
    std::cout << enum_name<ColorClass::RED>() << std::endl; // RED
}

// 获取枚举的最大值
template<typename T, std::size_t N = 0>
constexpr auto enum_max()
{
    constexpr auto value = static_cast<T>(N);
    if constexpr (enum_name<value>().find(')') == std::string_view::npos)
        return enum_max<T, N + 1>();
    else
        return N;
}

// 通过 make_index_sequence 打表获取枚举名数组
template<typename T>
requires std::is_enum_v<T> constexpr auto enum_name(T value)
{
    constexpr auto num   = enum_max<T>();
    constexpr auto names = []<std::size_t... Is>(std::index_sequence<Is...>)
    {
        return std::array<std::string_view, num> {enum_name<static_cast<T>(Is)>()...};
    }
    (std::make_index_sequence<num> {});
    return names[static_cast<std::size_t>(value)];
}

void dynamic_test()
{
    std::cout << "--------------------------------dynamic_test--------------------------------" << std::endl;
    // Trick: 当枚举值没有对应的枚举项时，打印的不是对应的枚举名，而是带有括号的强制转换表达式。
    // 注释关闭 clang 的 warning
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wenum-constexpr-conversion"
    constexpr Color c1 = static_cast<Color>(100);
#pragma clang diagnostic pop
    print_fn<c1>();
    // gcc and clang => void print_fn() [with auto T = (Color)100]
    // msvc => void __cdecl print_fn<100>(void)
    std::cout << enum_name<c1>() << std::endl; // (Color)100

    std::cout << enum_max<Color>() << std::endl;      // 3
    std::cout << enum_max<ColorClass>() << std::endl; // 3

    std::cout << enum_name(Color::RED) << std::endl;       // RED
    std::cout << enum_name(ColorClass::BLUE) << std::endl; // BLUE
}

int main()
{
    static_test();
    dynamic_test();
}