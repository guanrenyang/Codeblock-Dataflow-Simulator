#ifndef ROUTER_H
#define ROUTER_H

#include "VectorRegister.h"
#include "common.h"

class PEArray;
class RoutePackage;
class SPM;
class Inst;
class LoadSignalPackage;
class StoreDataPackage;

class Router : public std::enable_shared_from_this<Router>{
public:
    std::function<void(int, int, int, VectorData)> write_pe_reg;
    std::function<void(std::shared_ptr<LoadSignalPackage>)> read_shared_spm;
    std::function<void(std::shared_ptr<StoreDataPackage>)> write_shared_spm;

    Router(std::function<void(int, int, int, VectorData)> _write_pe_reg, \
           std::function<void(std::shared_ptr<LoadSignalPackage>)> _read_shared_spm, \
           std::function<void(std::shared_ptr<StoreDataPackage>)> _write_shared_spm) : \
           write_pe_reg(std::move(_write_pe_reg)), \
           read_shared_spm(std::move(_read_shared_spm)), \
           write_shared_spm(std::move(_write_shared_spm)){}

    void put(std::shared_ptr<RoutePackage> route_package);
    void issue_buffered_route_packages();
    void execute_cycle();
private:
    std::list<std::shared_ptr<RoutePackage>> route_packages;
    std::list<std::shared_ptr<RoutePackage>> buffered_route_packages;
    // function pointer to PEArray::write_pe_reg
};
#endif //ROUTER_H
