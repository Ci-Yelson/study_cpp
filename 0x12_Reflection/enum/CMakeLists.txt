add_executable(enum_magic_enum enum_magic_enum.cpp)

# vcpkg install magic-enum
find_package(magic_enum REQUIRED)
target_link_libraries(enum_magic_enum PRIVATE magic_enum::magic_enum)


# add_executable(old_my_enum old_my_enum.cpp)

add_executable(enum01 enum01.cpp)
add_executable(enum02 enum02.cpp)
add_executable(enum03 enum03.cpp)
