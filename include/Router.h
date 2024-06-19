#ifndef ROUTER_H
#define ROUTER_H

#include "VectorRegister.h"
#include "common.h"
#include <list>
#include <memory>
#include "RoutePackage.h"

class RoutePackage;
class PEArray;

class Router : public std::enable_shared_from_this<Router>{
public:
    std::function<void(int, int, int, VectorData)> write_pe_reg;
    Router(std::function<void(int, int, int, VectorData)> _write_pe_reg) : write_pe_reg(_write_pe_reg) {}

    void put(int pe_row_idx, int pe_col_idx, std::shared_ptr<RoutePackage> route_package){
        route_packages.push_back(route_package);
    };

    void execute_cycle() {
        // traverse the set
        for (auto it = route_packages.begin(); it != route_packages.end(); ) {
            (*it)->execute_cycle(shared_from_this());
            if ((*it)->is_completed()) {
                it = route_packages.erase(it);
            } else {
                ++it;
            }
        }
    }
private:
    std::list<std::shared_ptr<RoutePackage>> route_packages;
    // function pointer to PEArray::write_pe_reg
};
#endif //ROUTER_H
