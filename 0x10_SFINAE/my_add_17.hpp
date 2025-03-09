#pragma once

#include <type_traits>

// === 判断两个类型是否支持加法 === 
template<class T1, class T2, class = std::void_t<>>
struct is_add_support : std::false_type {};

// - 特化版本，当两个类型都支持加法时，为true - std::void_t，std::declval，std::declval<T1>() + std::declval<T2>()
template<class T1, class T2>
struct is_add_support<T1, T2, std::void_t<decltype(std::declval<T1>() + std::declval<T2>())>> : std::true_type {};

// #接口实现1 - std::enable_if_t - 普通类型的加法
template<class T1, class T2>
std::enable_if_t<is_add_support<T1, T2>::value, decltype(std::declval<T1>() + std::declval<T2>())>
my_add(T1 a, T2 b) {
    return a + b;
}

/// 条件触发流程
// 1. 函数模板实例化：当调用 my_add(a, b) 时
// 2. 触发特征检测：is_add_support<T1, T2>::value 需要求值
// 3. 尝试特化版本：template<class T1, class T2> struct is_add_support<T1, T2, std::void_t<decltype(std::declval<T1>() + std::declval<T2>())>> : std::true_type {};
// 4. 如果特化版本匹配成功，则启用特化版本，is_add_support<T1, T2>::value 为 true
// 5. 如果特化版本匹配失败，则启用原始模板：template<class T1, class T2, class = void> struct is_add_support<T1, T2, std::void_t<>> : std::false_type {};
//    此时 is_add_support<T1, T2>::value 为 false，my_add 函数模板被禁用


// === 判断两个类型是否都是容器类型 === 
template<class T1, class T2, class = std::void_t<>>
struct is_container : std::false_type {};

// - 特化版本，当两个类型都是容器类型时，为true
// - 判断逻辑：若支持迭代器，支持 begin() 和 end() 方法，则认为是容器类型
template<class T1, class T2>
struct is_container<T1, T2, std::void_t<class T1::iterator, class T1::iterator, decltype(std::declval<T1>().begin()), decltype(std::declval<T2>().end())>> : std::true_type {};

// === 判断一个类型是否有 emplace_back 方法 === 
template<class T, class = std::void_t<>>
struct has_emplace_back : std::false_type {};

// - 特化版本，当类型有 emplace_back 方法时，为true
template<class T>
struct has_emplace_back<T, std::void_t<decltype(std::declval<T>().emplace_back())>> : std::true_type {};

// // === 判断两个容器类型是否是相同的容器类型 - int简化版 === 
// template< template<class, class...> class C1, template<class, class...> class C2>
// struct is_same_container {
//     static constexpr bool value = std::is_same_v<C1<int>, C2<int>>;
// };

// #接口实现2 - 容器类型的加法
template<template<class, class...> class C1, class T1, class... Args1, 
         template<class, class...> class C2, class T2, class... Args2>
std::enable_if_t< is_container<C1<T1, Args1...>, C2<T2, Args2...>>::value, 
                  C1<decltype(std::declval<T1>() + std::declval<T2>()), Args1...> >
my_add(C1<T1, Args1...> a, C2<T2, Args2...> b) {
    C1<decltype(std::declval<T1>() + std::declval<T2>()), Args1...> ret;
    auto a_iter = a.begin();
    auto b_iter = b.begin();
    while (a_iter != a.end() && b_iter != b.end()) {
        if constexpr ( has_emplace_back<C1<T1>>::value && has_emplace_back<C2<T2>>::value ) {
            ret.emplace_back(*a_iter + *b_iter);
        } else {
            ret.push_back(*a_iter + *b_iter);
        }
        ++a_iter;
        ++b_iter;
    }
    while (a_iter != a.end()) {
        if constexpr ( has_emplace_back<C1<T1>>::value ) {
            ret.emplace_back(*a_iter);
        } else {
            ret.push_back(*a_iter);
        }
        ++a_iter;
    }
    while (b_iter != b.end()) {
        if constexpr ( has_emplace_back<C2<T2>>::value ) {
            ret.emplace_back(*b_iter);
        } else {
            ret.push_back(*b_iter);
        }
        ++b_iter;
    }
    return ret;
}


