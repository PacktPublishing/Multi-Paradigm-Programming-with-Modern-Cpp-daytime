cmake_minimum_required(VERSION 3.0.0)
project(example_daytime VERSION 0.1.0)

SET(Boost_USE_STATIC_LIBS ON)
FIND_PACKAGE( Boost 1.71 REQUIRED COMPONENTS system )
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

SET(CLANG_CPP_VERSION_FLAGS "-std=c++17")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_CPP_VERSION_FLAGS}")

include(CTest)
enable_testing()

add_executable(example_daytime src/main.cpp src/client.cpp)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

