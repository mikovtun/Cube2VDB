cmake_minimum_required(VERSION 3.22)
project(Cube2VDB)


file(GLOB SOURCES src/*.cpp)
file(GLOB HEADERS include/*.h)
add_executable(cube2vdb ${SOURCES} ${HEADERS})


target_include_directories(cube2vdb PUBLIC include )
target_link_libraries(cube2vdb PUBLIC blosc tbb openvdb )

set_property(TARGET cube2vdb PROPERTY CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
