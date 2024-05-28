cmake_minimum_required(VERSION 3.23)
project(Dataflow-Simulator)

set(CMAKE_CXX_STANDARD 17)

add_executable(Dataflow-Simulator main.cc
        CodeBlock.h
        common.h
        Inst.h
        PE.h
        LocalScheduler.h
        VectorRegister.h
)