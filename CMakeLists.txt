cmake_minimum_required(VERSION 3.23)
project(Dataflow-Simulator)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

file(GLOB SOURCES "src/*.cpp" "main.cc")

add_executable(Dataflow-Simulator ${SOURCES}
        src/VectorRegister.cpp)