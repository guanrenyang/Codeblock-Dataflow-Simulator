# Codeblock-Dataflow-Simulator

This is a codeblock-level dataflow simulator, designed for research purpose and supported by National Key R&D Program of China under Grant 2022YFB4501401.

## Compile && Run
```
mkdir build && cd build
cmake ..
make
./Dataflow-Simulator
```

## Add customized workloads
To add a customized workload, two functions are required:
1. Implement function`std::shared_ptr<DataFlowGraph> KERNEL_NAME(PEArray & pe_array);` (declared in `main.cc`), where `KERNEL_NAME` is a macro representing the function name and should be modified accordingly.
2. Implement `int display(PEArray & pe_array, int cycle);`. If you don't need to display anything, define it as empty.

**Suggestions for better project structure:** Put the definitions of the two functions above into `kernel/KERNEL_NAME.cpp`, and add `kernel/KERNEL_NAME.cpp` to CMakeLists.txt.


