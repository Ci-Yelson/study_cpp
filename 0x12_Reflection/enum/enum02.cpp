#include <iostream>

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

#include "enum02_autogen.inc"

int main()
{
    std::cout << Color_to_string(Color::RED) << std::endl;
    std::cout << Color_to_string(Color::GREEN) << std::endl;
    std::cout << Color_to_string(Color::BLUE) << std::endl;

    std::cout << ColorClass_to_string(ColorClass::RED) << std::endl;
    std::cout << ColorClass_to_string(ColorClass::GREEN) << std::endl;
    std::cout << ColorClass_to_string(ColorClass::BLUE) << std::endl;

    return 0;
}