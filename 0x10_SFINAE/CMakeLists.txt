cmake_minimum_required(VERSION 3.22)

# 如果没有使用 vcpkg integrate install，需要显式指定工具链文件
# 取消下面这行的注释，并替换路径 【注意须在 project 之前】
set(CMAKE_TOOLCHAIN_FILE "~/No.Softwares/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(sfinae)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置导出 compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 创建可执行文件
add_executable(${PROJECT_NAME} my_add_17_test.cpp my_add_17.hpp)