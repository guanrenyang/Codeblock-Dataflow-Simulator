#ifndef ROUTER_H
#define ROUTER_H

#include "VectorRegister.h"
#include "common.h"
#include <list>
#include <memory>
class PEArray;
class RoutePackage;
class Router : public std::enable_shared_from_this<Router>{
public:
    std::function<void(int, int, int, VectorData)> write_pe_reg;

    Router(std::function<void(int, int, int, VectorData)> _write_pe_reg) : write_pe_reg(_write_pe_reg) {}
    void put(int pe_row_idx, int pe_col_idx, std::shared_ptr<RoutePackage> route_package);
    void execute_cycle();
private:
    std::list<std::shared_ptr<RoutePackage>> route_packages;
    // function pointer to PEArray::write_pe_reg
};
#endif //ROUTER_H
