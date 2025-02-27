cmake_minimum_required(VERSION 3.22)

# 如果没有使用 vcpkg integrate install，需要显式指定工具链文件
# 取消下面这行的注释，并替换路径 【注意须在 project 之前】
set(CMAKE_TOOLCHAIN_FILE "/root/No.Softwares/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(tbb_example)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置导出 compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# if(NOT CMAKE_BUILD_TYPE)
#     set(CMAKE_BUILD_TYPE Release)
# endif()

# add_compile_options(-fsanitize=thread -g -fno-omit-frame-pointer)
# add_link_options(-fsanitize=thread)

# 查找 TBB 包
find_package(TBB REQUIRED)

# 查找 benchmark 包
find_package(benchmark REQUIRED)

# 创建可执行文件
add_executable(tbb_example tbb_deadlock.cpp)
# add_executable(tbb_example tbb_divide_conquer.cpp)
# add_executable(tbb_example tbb_sort.cpp)

# 链接 TBB 库
target_link_libraries(tbb_example PUBLIC TBB::tbb) 

# 链接 benchmark 库
target_link_libraries(tbb_example PUBLIC benchmark::benchmark)