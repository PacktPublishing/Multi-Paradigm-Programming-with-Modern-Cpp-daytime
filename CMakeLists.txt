cmake_minimum_required(VERSION 3.0.0)
project(example_daytime VERSION 0.1.0)

SET(Boost_NO_BOOST_CMAKE ON)
SET(Boost_USE_STATIC_LIBS OFF) 
SET(Boost_USE_MULTITHREADED ON)  
SET(Boost_USE_STATIC_RUNTIME OFF) 
FIND_PACKAGE( Boost 1.71 REQUIRED COMPONENTS system )
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

SET(CLANG_CPP_VERSION_FLAGS "-std=c++17")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CLANG_CPP_VERSION_FLAGS}")

include(CTest)
enable_testing()

# Packages that the application depends on
add_subdirectory(src/daytime)
add_subdirectory(src/helpers)

add_executable(example_daytime src/main.cpp)
target_include_directories(example_daytime PRIVATE src/daytime)
target_link_libraries(example_daytime daytime helpers)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

