cmake_minimum_required(VERSION 4.2.3)

project(strings LANGUAGES CXX)

add_library(${PROJECT_NAME} INTERFACE)
add_library(utils::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_23)

target_sources(${PROJECT_NAME}
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
        include/utils/strings/concepts.hpp
        include/utils/strings/char_classification.hpp
        include/utils/strings/char_transform.hpp
        include/utils/strings/transform_trim.hpp
        include/utils/strings.hpp
)

target_link_libraries(${PROJECT_NAME} INTERFACE utils::platform)

#if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
#    target_compile_options(${PROJECT_NAME} INTERFACE -mavx2 -mbmi -mbmi2 -mlzcnt)
#elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
#    target_compile_options(${PROJECT_NAME} INTERFACE /arch:AVX2)
#endif()

#include(GNUInstallDirs)
#install(TARGETS ${PROJECT_NAME}
#    EXPORT ${PROJECT_NAME}-targets
#    FILE_SET HEADERS
#)
#install(EXPORT ${PROJECT_NAME}-targets
#    NAMESPACE utils::
#    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
#)
