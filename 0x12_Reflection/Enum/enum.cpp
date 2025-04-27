#include <magic_enum/magic_enum.hpp>
#include <iostream>
#include <format>

enum class Color : int { RED = -10, BLUE = 0, GREEN = 1000000 };

template <>
struct magic_enum::customize::enum_range<Color> {
  static constexpr int min = -10;
  static constexpr int max = 1000000; // Cannot work with 1000000
  // (max - min) must be less than UINT16_MAX.
};

int main() {
  // 1. 获取enum值的名称 - enum_name(enum_value)
  std::cout << "1. 获取enum值的名称" << std::endl;
  std::cout << magic_enum::enum_name(Color::RED) << std::endl;    // RED
  std::cout << magic_enum::enum_name(Color::BLUE) << std::endl;   // BLUE
  std::cout << magic_enum::enum_name(Color::GREEN) << std::endl;  // GREEN

  // 2. 将字符串转换为enum值 - enum_cast<enum_type>(string)
  std::cout << "2. 将字符串转换为enum值" << std::endl;
  auto red = magic_enum::enum_cast<Color>("RED");
  if (red.has_value()) {
    std::cout << std::format("int: {}, name: {}", int(red.value()), magic_enum::enum_name(red.value())) << std::endl;
    // int: -10, name: RED
  }
  auto blue = magic_enum::enum_cast<Color>("BLUE");
  if (blue.has_value()) {
    std::cout << std::format("int: {}, name: {}", int(blue.value()), magic_enum::enum_name(blue.value())) << std::endl;
    // int: 0, name: BLUE
  }
  auto green = magic_enum::enum_cast<Color>("GREEN");
  if (green.has_value()) {
    std::cout << std::format("int: {}, name: {}", int(green.value()), magic_enum::enum_name(green.value())) << std::endl;
    // int: 10, name: GREEN
  }

  // 3. 获取该enum类型的全部值 - enum_values<Color>()
  std::cout << "3. 获取该enum类型的全部值" << std::endl;
  for (auto color : magic_enum::enum_values<Color>()) {
    std::cout << std::format("int: {}, name: {}", int(color), magic_enum::enum_name(color)) << std::endl;
  }
  
  // 4. 获取enum值的个数 - enum_count<enum_type>()
  std::cout << "4. 获取enum值的个数" << std::endl;
  std::cout << magic_enum::enum_count<Color>() << std::endl;

  // 5. 获取enum值的名称 - enum_values<enum_type>()
  std::cout << "5. 获取enum值的名称" << std::endl;
  for (auto color_name : magic_enum::enum_names<Color>()) {
    std::cout << std::format("name: {}", color_name) << std::endl;
  }

  // 6. 获取该enum类型的值-名称对(entry)列表
  std::cout << "6. 获取该enum类型的值-名称对(entry)列表" << std::endl;
  for (auto [key, value] : magic_enum::enum_entries<Color>()) {
    std::cout << std::format("key: {}, value: {}", int(key), value) << std::endl;
  }
}