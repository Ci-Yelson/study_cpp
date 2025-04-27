#include <bits/stdc++.h>
using namespace std;

struct str_view {
    const char* str = nullptr;
    size_t len = 0;
};

template <class T, T V>
constexpr auto get_enum_name_helper() {
    constexpr auto info = __PRETTY_FUNCTION__;
    str_view name;
    name.str = info;
    name.len = sizeof(info);
    return name;
}

enum MyEnum { A, B, C };
enum class MyEnumClass { A, B, C };

constexpr auto get_enum_count() {
    size_t count = 0;
    #define VALID_ENUM(e) if constexpr (get_enum_name_helper<MyEnum, static_cast<MyEnum>(e)>().len > 0) { count++; }
    VALID_ENUM(0);
    VALID_ENUM(1);
    VALID_ENUM(2);
    #undef VALID_ENUM
    return count;
}

template <class T>
constexpr T get_enum_value(size_t index) {
    return static_cast<T>(index);
}

constexpr auto get_enums() {
    std::array<MyEnum, get_enum_count()> enums;
    for (size_t i = 0; i < get_enum_count(); i++) {
        enums[i] = get_enum_value<MyEnum>(i);
    }
    return enums;
}

// 用 index_sequence 展开
// 利用 std::index_sequence 展开所有可能的枚举值，在编译期生成所有名字。
template <std::size_t... I>
constexpr auto get_enum_names_impl(std::index_sequence<I...>) {
    return std::array<str_view, sizeof...(I)>{
        get_enum_name_helper<MyEnum, static_cast<MyEnum>(I)>()...
    };
}

constexpr auto get_enum_names() {
    return get_enum_names_impl(std::make_index_sequence<get_enum_count()>{});
}


constexpr auto get_enum_name(MyEnum v) {
    return get_enum_names()[static_cast<std::size_t>(v)];
}

int main() {
    // cout << get_enum_name_helper<MyEnum, MyEnum::A>().str << endl;
    // cout << get_enum_name_helper<MyEnumClass, MyEnumClass::A>().str << endl;

    for (auto e : get_enums()) {
        cout << get_enum_name(e).str << endl;
    }
    cout << get_enum_name(MyEnum::A).str << endl;
    return 0;
}

