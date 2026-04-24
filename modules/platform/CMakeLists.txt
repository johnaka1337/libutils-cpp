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
        include/utils/platform/types.hpp
        include/utils/platform/cpu/registers.hpp
        include/utils/platform/cpu/cpuid.hpp
        include/utils/platform/cpu/features_x86-64.hpp
        include/utils/platform/cpu/features_arm.hpp
        include/utils//platform/cpu/features.hpp

        include/utils/platform.hpp
)

option(BUILD_${PROJECT_NAME}_TEST "${PROJECT_NAME} tests" OFF)
if(BUILD_${PROJECT_NAME}_TEST OR BUILD_TESTS)
    add_executable(${PROJECT_NAME}_test src/${PROJECT_NAME}_test.cpp)
    target_link_libraries(${PROJECT_NAME}_test PRIVATE ${PROJECT_NAME})
endif()

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
