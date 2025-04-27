
add_executable(enum enum.cpp)

# vcpkg install magic-enum
find_package(magic_enum REQUIRED)
target_link_libraries(enum PRIVATE magic_enum::magic_enum)


add_executable(my_enum my_enum.cpp)
