cmake_minimum_required(VERSION 4.2.3)

project(platform LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)
add_library(utils::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_23)

target_sources(${PROJECT_NAME}
    PUBLIC # IDE DX workaround: Must be INTERFACE conceptually, but PUBLIC shows files in project tree
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
        include/utils//platform/compiler.hpp
        include/utils/platform/types.hpp
        include/utils/platform/features.hpp
        include/utils/platform/simd.hpp
        include/utils/platform.hpp
)

#if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
#    target_compile_options(${PROJECT_NAME} INTERFACE -mavx2 -mbmi -mbmi2 -mlzcnt)
#elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
#    target_compile_options(${PROJECT_NAME} INTERFACE /arch:AVX2)
#endif()

#include(GNUInstallDirs)
#install(TARGETS platform
#    EXPORT platform-targets
#    FILE_SET HEADERS
#)
#install(EXPORT platform-targets
#    NAMESPACE utils::
#    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/platform
#)
