# abseil.cmake

if(TARGET absl::base)
    return()
endif()

include(CPM)

# 设置 abseil 版本
set(ABSL_VERSION "20250127.0")

CPMAddPackage(
    NAME abseil
    VERSION ${ABSL_VERSION}
    GITHUB_REPOSITORY abseil/abseil-cpp
    GIT_TAG ${ABSL_VERSION}
    OPTIONS 
        "ABSL_ENABLE_INSTALL ON"
        "ABSL_USE_EXTERNAL_GOOGLETEST OFF"
        "ABSL_PROPAGATE_CXX_STD ON"
        "BUILD_TESTING OFF"
)

if(abseil_ADDED)
    message(STATUS "Added abseil ${ABSL_VERSION}")
else()
    message(STATUS "Using existing abseil ${ABSL_VERSION}")
endif() 