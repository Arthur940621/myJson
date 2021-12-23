cmake_minimum_required(VERSION 3.0)

project(myJson)

include_directories(include)

add_compile_options(-Wall -std=c++17)

set(CMAKE_BUILD_TYPE Debug)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

file(GLOB SRC ./src/*.cpp)
add_library(myJson SHARED ${SRC})

enable_testing()
add_executable(Test ./test/test.cpp)
target_link_libraries(Test myJson /usr/local/lib/libgtest.a /usr/local/lib/ libgtest_main.a -pthread)

add_executable(Jsonchecker ./test/jsonchecker.cpp)
target_link_libraries(Jsonchecker myJson)