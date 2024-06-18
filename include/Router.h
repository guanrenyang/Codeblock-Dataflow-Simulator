#ifndef ROUTER_H
#define ROUTER_H

#include "common.h"
class RoutePackage;

class Router {
public:
    void put(int pe_row_idx, int pe_col_idx, RoutePackage route_package);
    void execute_cycle(); // perform the operations of each route package
private:
    std::vector<RoutePackage> route_package_on_chip;

};
#endif //ROUTER_H
