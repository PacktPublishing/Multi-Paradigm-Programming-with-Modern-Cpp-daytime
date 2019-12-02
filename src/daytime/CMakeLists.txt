add_library(daytime src/client.cpp src/daytime_server.cpp)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../network")