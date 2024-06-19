#ifndef DATAFLOW_SIMULATOR_ROUTEPACKAGE_H
#define DATAFLOW_SIMULATOR_ROUTEPACKAGE_H

#include "Inst.h"
#include "LocalScheduler.h"
#include "Router.h"
#include "VectorRegister.h"
#include "common.h"
#include <istream>
#include <memory>

class Router;
class RoutePackage {
private:
    std::shared_ptr<AsyncInstManager> _async_inst_manager; // source pe
    std::shared_ptr<Inst> _inst; // source inst
    bool completed = false;
public:
    int remaining_hops;
    RoutePackage(std::shared_ptr<AsyncInstManager> async_inst_manager, std::shared_ptr<Inst> inst) : _async_inst_manager(async_inst_manager), _inst(inst) {}

    virtual void operate(std::shared_ptr<Router> router) = 0; // perform the operations of each route package
    void execute_cycle(std::shared_ptr<Router> router) { // perform the operations of each route package
        if (remaining_hops > 0) {
            remaining_hops--;
        } else {
            operate(router);
            _async_inst_manager->remove_async_inst(_inst);
            completed = true;
        }
    } 
    bool is_completed(){
        return completed;
    }
};

class CopyDataPackage: public RoutePackage {
public:
    CopyDataPackage(int src_pe_row, int src_pe_col, int dst_pe_row, int dst_pe_col, int reg_idx, std::shared_ptr<AsyncInstManager> async_inst_manager, std::shared_ptr<Inst> inst) : RoutePackage(async_inst_manager, inst) {
        this->dst_pe_row_idx = dst_pe_row;
        this->dst_pe_col_idx = dst_pe_col;
        this->reg_idx = reg_idx;
        this->remaining_hops = std::abs(src_pe_row - dst_pe_row) + std::abs(src_pe_col - dst_pe_col);
    }

    void operate(std::shared_ptr<Router> router) override {
        router->write_pe_reg(dst_pe_row_idx, dst_pe_col_idx, 0, data);
    } 

    int dst_pe_row_idx;
    int dst_pe_col_idx;
    int reg_idx;
    VectorData data;
};

class LoadSignalPackage: public RoutePackage {
    // Read SPM
};

class LoadDataPackage: public RoutePackage {
    // Write to reg
};

class StoreDataPackage: public RoutePackage {
    // Write to SPM
};

class StoreAckPackage: public RoutePackage {

};
#endif //DATAFLOW_SIMULATOR_ROUTEPACKAGE_H
