cmake_minimum_required(VERSION 3.23)
project(Dataflow-Simulator)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

file(GLOB SOURCES "src/*.cpp")

add_executable(Dataflow-Simulator ${SOURCES} main.cc)

add_executable(Dataflow-Test ${SOURCES} "${CMAKE_CURRENT_SOURCE_DIR}/test/test_load_sotre.cc")