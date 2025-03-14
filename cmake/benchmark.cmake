# benchamrk (https://github.com/google/benchmark)
# License: Apache License 2.0

if(TARGET benchmark::benchmark)
    return()
endif()

# 使用 CPM 添加 Google Benchmark 依赖
include(CPM)
CPMAddPackage(
  NAME benchmark
  GITHUB_REPOSITORY google/benchmark
  VERSION 1.9.1
  OPTIONS
    "BENCHMARK_ENABLE_TESTING OFF"
    "BENCHMARK_ENABLE_INSTALL OFF"
)

if(benchmark_ADDED)
  # 添加编译选项以提高性能
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(benchmark PRIVATE -O3)
  elseif(MSVC)
    target_compile_options(benchmark PRIVATE /O2)
  endif()
endif()
