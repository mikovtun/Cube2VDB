cmake_minimum_required(VERSION 3.00)
project(Cube2VDB)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
add_executable(cube2vdb src/main.cpp)

find_package(Threads REQUIRED)
find_library(Half_LIB Half REQUIRED)

target_include_directories(cube2vdb PUBLIC include ${tbb_INCLUDE_DIR} )
target_link_libraries(cube2vdb PUBLIC blosc tbb openvdb Threads::Threads ${Half_LIB} )

set_property(TARGET cube2vdb PROPERTY CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
