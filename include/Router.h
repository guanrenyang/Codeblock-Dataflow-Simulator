#ifndef ROUTER_H
#define ROUTER_H

#include "PEArray.h"
#include "VectorRegister.h"
#include "common.h"
#include <memory>
class RoutePackage;

template<int _num_row, int _num_col, int _memory_size>
class Router {
public:
    //TODO: add a method to get the route package at a specific PE
    void put(int pe_row_idx, int pe_col_idx, std::shared_ptr<RoutePackage> route_package);
    void set(int pe_row_idx, int pe_col_idx, int reg_idx, VectorData vector_data); // set specific pe reg
    //TODO: perform the operations of each route package
    void execute_cycle(); // perform the operations of each route package
private:
    std::vector<RoutePackage> route_package_on_chip;
};
#endif //ROUTER_H
