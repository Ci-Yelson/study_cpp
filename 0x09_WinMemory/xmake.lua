-- 设置项目信息
set_project("mem")
set_version("1.0")

-- 设置C++标准
set_languages("cxx17")

-- 设置构建模式
add_rules("mode.release", "mode.debug")

-- 添加目标
target("mem")
    set_kind("binary")
    add_files("mem.cpp")
    
    -- 添加Eigen3依赖
    add_packages("vcpkg::eigen3")
    add_defines("EIGEN_DONT_PARALLELIZE")

    -- 在Windows平台添加psapi库
    if is_plat("windows") then
        add_syslinks("psapi")
    end 