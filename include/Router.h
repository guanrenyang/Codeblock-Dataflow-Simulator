#ifndef ROUTER_H
#define ROUTER_H

#include "VectorRegister.h"
#include "common.h"
#include <list>
#include <memory>
#include <utility>
class PEArray;
class RoutePackage;
class SPM;

class Router : public std::enable_shared_from_this<Router>{
public:
    std::function<void(int, int, int, VectorData)> write_pe_reg;
    std::function<VectorData(uint32_t)> read_shared_spm;
    std::function<void(uint32_t, VectorData)> write_shared_spm;

    Router(std::function<void(int, int, int, VectorData)> _write_pe_reg, \
           std::function<VectorData(uint32_t)> _read_shared_spm, \
           std::function<void(uint32_t, VectorData)> _write_shared_spm) : \
           write_pe_reg(std::move(_write_pe_reg)), \
           read_shared_spm(std::move(_read_shared_spm)), \
           write_shared_spm(std::move(_write_shared_spm)){}

    void put(int pe_row_idx, int pe_col_idx, std::shared_ptr<RoutePackage> route_package);
    void execute_cycle();
private:
    std::list<std::shared_ptr<RoutePackage>> route_packages;
    // function pointer to PEArray::write_pe_reg
};
#endif //ROUTER_H
