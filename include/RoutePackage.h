#ifndef DATAFLOW_SIMULATOR_ROUTEPACKAGE_H
#define DATAFLOW_SIMULATOR_ROUTEPACKAGE_H

#include "LocalScheduler.h"
#include "VectorRegister.h"
#include "common.h"
class RoutePackage {
private:
    AsyncInstManager& _async_inst_manager; // source pe

public:
    int remaining_hops;
    RoutePackage(AsyncInstManager& async_inst_manager) : _async_inst_manager(async_inst_manager) {}

    virtual void operate() = 0; // perform the operations of each route package
    void execute_cycle() { // perform the operations of each route package
        if (remaining_hops > 0) {
            remaining_hops--;
        } else {
            operate();
        }
    } 
};

class CopyDataPackage: public RoutePackage {
public:
    CopyDataPackage(int src_pe_row, int src_pe_col, int dst_pe_row, int dst_pe_col, AsyncInstManager& async_inst_manager) : RoutePackage(async_inst_manager) {
        remaining_hops = std::abs(src_pe_row - dst_pe_row) + std::abs(src_pe_col - dst_pe_col);
    }

    void operate() override {

    } 

    int dst_pe_row_idx;
    int dst_pe_col_idx;
    VectorData data;

    // Write to reg
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
