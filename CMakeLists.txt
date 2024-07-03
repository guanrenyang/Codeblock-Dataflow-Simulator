cmake_minimum_required(VERSION 3.16)

project(Dataflow-Simulator)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

add_executable(Dataflow-Simulator ${SOURCES} main.cc)

add_executable(Dataflow-Test ${SOURCES} "${CMAKE_CURRENT_SOURCE_DIR}/test/test_load_store.cc")