cmake_minimum_required(VERSION 3.22)

# 如果没有使用 vcpkg integrate install，需要显式指定工具链文件
# 取消下面这行的注释，并替换路径 【注意须在 project 之前】
set(CMAKE_TOOLCHAIN_FILE "~/No.Softwares/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(simd)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置导出 compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 若为 Linux 系统，则启用 SSE 指令集
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse3")
endif()

# 查找 benchmark 包 - vcpkg install benchmark
find_package(benchmark REQUIRED)

# 查找 Eigen3 包 - vcpkg install eigen3
find_package(Eigen3 REQUIRED)

# 创建可执行文件
add_executable(simd simd_mat44.cpp)

# 链接库
target_link_libraries(${PROJECT_NAME} PUBLIC benchmark::benchmark)
target_link_libraries(${PROJECT_NAME} PUBLIC Eigen3::Eigen)