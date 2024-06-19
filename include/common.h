#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <array>
#include <memory>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <set>
#include <queue>

enum class DataType {
    INT8,
    INT16,
    INT32,
    FLOAT8,
    FLOAT16,
    FLOAT32,
    FLOAT64,
};

enum class InstType {
    FP32_ADD,
    FP32_MUL,
};

#endif //COMMON_H