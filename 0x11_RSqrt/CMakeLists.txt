cmake_minimum_required(VERSION 3.22)

# 如果没有使用 vcpkg integrate install，需要显式指定工具链文件
# 取消下面这行的注释，并替换路径 【注意须在 project 之前】
# set(CMAKE_TOOLCHAIN_FILE "~/No.Softwares/vcpkg/scripts/buildsystems/vcpkg.cmake")

# Set default minimum C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置导出 compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(rsqrt)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 若为 Linux 系统，则启用 SSE 指令集
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -mfma")
endif()

if(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm64" AND APPLE)
    set(POLYSOLVE_NOT_ON_APPLE_SILICON OFF)
    set(POLYSOLVE_ON_APPLE_SILICON ON)
else()
    set(POLYSOLVE_NOT_ON_APPLE_SILICON ON)
    set(POLYSOLVE_ON_APPLE_SILICON OFF)
endif()

################################################################################
option(POLYSOLVE_WITH_ACCELERATE    "Enable Apple Accelerate" ${POLYSOLVE_ON_APPLE_SILICON})
option(POLYSOLVE_WITH_MKL           "Enable MKL library"  ${POLYSOLVE_NOT_ON_APPLE_SILICON})

# include(CMakeDependentOption)
# cmake_dependent_option(EIGEN_WITH_MKL "Use Eigen with MKL" ON "POLYSOLVE_WITH_MKL" OFF)
add_compile_definitions(EIGEN_WITH_MKL)

# Since MKL comes precompiled with /MD on Windows, we need to use the same MSVC runtime library flag
# globally for the whole project (otherwise in Debug we will have a mismatch between /MD and /MDd).
if(POLYSOLVE_WITH_MKL)
    # Set MSVC runtime library globally for all targets
    message(STATUS "Forcing /MD globally due MKL being enabled")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL" CACHE STRING "Select the MSVC runtime library")
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif()

### Configuration
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../cmake")

# General CMake utils
include(cpm_cache)

################################################################################
# 创建可执行文件
################################################################################
add_executable(${PROJECT_NAME} rsqrt.cpp)

################################################################################
# Required libraries
################################################################################

# 查找 benchmark 包 - vcpkg install benchmark
include(benchmark)
target_link_libraries(${PROJECT_NAME} PUBLIC benchmark::benchmark)

# # 查找 Eigen3 包 - vcpkg install eigen3
# include(eigen)
# target_link_libraries(${PROJECT_NAME} PUBLIC Eigen3::Eigen)
# add_compile_definitions(EIGEN_DONT_PARALLELIZE)

# # 查找 libigl 包 - vcpkg install libigl
# include(libigl)
# target_link_libraries(${PROJECT_NAME} PRIVATE igl::core)
# # target_link_libraries(${PROJECT_NAME} PRIVATE igl::igl_core igl_copyleft::igl_copyleft_core)

# 查找 spdlog 包 - vcpkg install spdlog
include(spdlog)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)

# # 查找 tbb 包 - vcpkg install tbb
# include(onetbb)
# target_link_libraries(${PROJECT_NAME} PUBLIC TBB::tbb)

# # MKL library
# if(POLYSOLVE_WITH_MKL)
#     include(mkl)
#     target_link_libraries(${PROJECT_NAME} PRIVATE mkl::mkl)
# endif()