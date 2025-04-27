#include <magic_enum/magic_enum.hpp>
#include <iostream>
#include <format>

enum class Color : int { RED = -10, BLUE = 0, GREEN = 10 };

int main() {
  std::cout << magic_enum::enum_name(Color::RED) << std::endl;    // RED
}


// Returns name from enum value.
// If enum value does not have name or value out of range, returns empty string.
template <typename E, detail::enum_subtype S = detail::subtype_v<E>>
[[nodiscard]] constexpr auto enum_name(E value) noexcept -> detail::enable_if_t<E, string_view> {
  using D = std::decay_t<E>;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum requires enum implementation and valid max and min.");

  if (const auto i = enum_index<D, S>(value)) {
    return detail::names_v<D, S>[*i];
  }
  return {};
}

////////////////////////////////////////////////////////////////////////////////////////
enum class enum_subtype {
  common,
  flags
};

template <typename E, typename U = std::underlying_type_t<E>>
constexpr enum_subtype subtype(std::true_type) noexcept {
  if constexpr (std::is_same_v<U, bool>) { // bool special case
    return enum_subtype::common;
  } else if constexpr (has_is_flags<E>::value) {
    return customize::enum_range<E>::is_flags ? enum_subtype::flags : enum_subtype::common;
  } else {
#if defined(MAGIC_ENUM_AUTO_IS_FLAGS)
    constexpr auto flags_values = values<E, enum_subtype::flags>();
    constexpr auto default_values = values<E, enum_subtype::common>();
    if (flags_values.size() == 0 || default_values.size() > flags_values.size()) {
      return enum_subtype::common;
    }
    for (std::size_t i = 0; i < default_values.size(); ++i) {
      const auto v = static_cast<U>(default_values[i]);
      if (v != 0 && (v & (v - 1)) != 0) {
        return enum_subtype::common;
      }
    }
    return enum_subtype::flags;
#else
    return enum_subtype::common;
#endif
  }
}

template <typename T>
constexpr enum_subtype subtype(std::false_type) noexcept {
  // For non-enum type return default common subtype.
  return enum_subtype::common;
}

template <typename E, typename D = std::decay_t<E>>
inline constexpr auto subtype_v = subtype<D>(std::is_enum<D>{});
////////////////////////////////////////////////////////////////////////////////////////


// Obtains index in enum values from static storage enum variable.
template <auto V, detail::enum_subtype S = detail::subtype_v<std::decay_t<decltype(V)>>>
[[nodiscard]] constexpr auto enum_index() noexcept -> detail::enable_if_t<decltype(V), std::size_t> {\
  using D = std::decay_t<decltype(V)>;
  static_assert(detail::is_reflected_v<D, S>, "magic_enum requires enum implementation and valid max and min.");
  constexpr auto index = enum_index<D, S>(V);
  static_assert(index, "magic_enum::enum_index enum value does not have a index.");

  return *index;
}


////////////////////////////////////////////////////////////////////////////////////////
// names<E, S>() -> enum_name<E, values_v<E, S>[I]>() -> n<V>()
// 
// values_v<E, S>() -> values<E, S>() -> values<E, S, range_size, min>() -> value<E, Min, S>(i) -> static_cast<E>(ualue<E, O, S>(i))
//
////////////////////////////////////////////////////////////////////////////////////////
template <typename E, enum_subtype S, std::size_t... I>
constexpr auto names(std::index_sequence<I...>) noexcept {
  constexpr auto names = std::array<string_view, sizeof...(I)>{{enum_name_v<E, values_v<E, S>[I]>...}};
  return names;
}

template <typename E, enum_subtype S>
inline constexpr auto names_v = names<E, S>(std::make_index_sequence<count_v<E, S>>{});


template <typename E, E V>
inline constexpr auto enum_name_v = enum_name<E, V>();

template <typename E, E V>
constexpr auto enum_name() noexcept {
  [[maybe_unused]] constexpr auto custom = customize::enum_name<E>(V);
  static_assert(std::is_same_v<std::decay_t<decltype(custom)>, customize::customize_t>, "magic_enum::customize requires customize_t type.");
  if constexpr (custom.first == customize::detail::customize_tag::custom_tag) {
    constexpr auto name = custom.second;
    static_assert(!name.empty(), "magic_enum::customize requires not empty string.");
    return static_str<name.size()>{name};
  } else if constexpr (custom.first == customize::detail::customize_tag::invalid_tag) {
    return static_str<0>{};
  } else if constexpr (custom.first == customize::detail::customize_tag::default_tag) {
#if defined(MAGIC_ENUM_VS_2017_WORKAROUND)
    constexpr auto name = n<E, V>();
#else
    constexpr auto name = n<V>();
#endif
    return static_str<name.size_>{name};
  } else {
    static_assert(always_false_v<E>, "magic_enum::customize invalid.");
  }
}

template <auto V>
constexpr auto n() noexcept {
  static_assert(is_enum_v<decltype(V)>, "magic_enum::detail::n requires enum type.");

  if constexpr (supported<decltype(V)>::value) {
#if defined(MAGIC_ENUM_GET_ENUM_NAME_BUILTIN)
    constexpr auto name_ptr = MAGIC_ENUM_GET_ENUM_NAME_BUILTIN(V);
    auto name = name_ptr ? str_view{name_ptr, std::char_traits<char>::length(name_ptr)} : str_view{};
#elif defined(__clang__)
    str_view name;
    if constexpr (sizeof(__PRETTY_FUNCTION__) == sizeof(__FUNCTION__)) {
      static_assert(always_false_v<decltype(V)>, "magic_enum::detail::n requires __PRETTY_FUNCTION__.");
      return str_view{};
    } else {
      name.size_ = sizeof(__PRETTY_FUNCTION__) - 36;
      name.str_ = __PRETTY_FUNCTION__ + 34;
    }
    if (name.size_ > 22 && name.str_[0] == '(' && name.str_[1] == 'a' && name.str_[10] == ' ' && name.str_[22] == ':') {
      name.size_ -= 23;
      name.str_ += 23;
    }
    if (name.str_[0] == '(' || name.str_[0] == '-' || (name.str_[0] >= '0' && name.str_[0] <= '9')) {
      name = str_view{};
    }
#elif defined(__GNUC__)
    auto name = str_view{__PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 1};
    if constexpr (sizeof(__PRETTY_FUNCTION__) == sizeof(__FUNCTION__)) {
      static_assert(always_false_v<decltype(V)>, "magic_enum::detail::n requires __PRETTY_FUNCTION__.");
      return str_view{};
    } else if (name.str_[name.size_ - 1] == ']') {
      name.size_ -= 55;
      name.str_ += 54;
    } else {
      name.size_ -= 40;
      name.str_ += 37;
    }
    if (name.str_[0] == '(') {
      name = str_view{};
    }
#elif defined(_MSC_VER)
    str_view name;
    if ((__FUNCSIG__[5] == '_' && __FUNCSIG__[35] != '(') || (__FUNCSIG__[5] == 'c' && __FUNCSIG__[41] != '(')) {
      // CLI/C++ workaround (see https://github.com/Neargye/magic_enum/issues/284).
      name.str_ = __FUNCSIG__;
      name.str_ += 35;
      name.size_ = sizeof(__FUNCSIG__) - 52;
    }
#else
    auto name = str_view{};
#endif
    std::size_t p = 0;
    for (std::size_t i = name.size_; i > 0; --i) {
      if (name.str_[i] == ':') {
        p = i + 1;
        break;
      }
    }
    if (p > 0) {
      name.size_ -= p;
      name.str_ += p;
    }
    return name;
  } else {
    return str_view{}; // Unsupported compiler or Invalid customize.
  }
}