cmake_minimum_required(VERSION 4.2.3)

project(libutils-cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


option(BUILD_TESTS "Build tests" ON)

add_subdirectory(modules/platform/)
add_subdirectory(modules/strings/)

add_executable(utils main.cpp)
target_link_libraries(utils PRIVATE utils::platform utils::strings)
#option(BUILD_EXAMPLES "Build examples" OFF)
#if(BUILD_EXAMPLES)
#    #add_executable(`)
#endif()

#include(GNUInstallDirs)
#install(TARGETS libutils-cpp
#    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#)
