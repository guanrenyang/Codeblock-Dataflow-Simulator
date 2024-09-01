cmake_minimum_required(VERSION 3.16)

project(Dataflow-Simulator)

set(CMAKE_CXX_STANDARD 14)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG=1)
endif()

include_directories(include)

file(GLOB SOURCES "src/*.cpp")
file(GLOB KERNELS "kernels/*.cpp")

add_executable(Dataflow-Simulator ${SOURCES} ${KERNELS} main.cc)

add_executable(Dataflow-Test ${SOURCES} ${KERNELS} "${CMAKE_CURRENT_SOURCE_DIR}/test/test_parallel_codeblock.cpp")
add_executable(tensor-core-test ${SOURCES} "${CMAKE_CURRENT_SOURCE_DIR}/test/test_tensor_core.cpp")