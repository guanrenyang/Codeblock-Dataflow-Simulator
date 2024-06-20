#ifndef DATAFLOW_SIMULATOR_ROUTEPACKAGE_H
#define DATAFLOW_SIMULATOR_ROUTEPACKAGE_H

#include "common.h"

class Inst;
class AsyncInstManager;
class Router;

typedef std::array<uint8_t, 128> VectorData;

class RoutePackage {
private:
    std::shared_ptr<AsyncInstManager> _async_inst_manager; // source pe
    std::shared_ptr<Inst> _inst; // source inst
    bool completed = false;
public:
    int remaining_hops;
    RoutePackage(std::shared_ptr<Inst> inst) ;

    virtual void operate(std::shared_ptr<Router> router) = 0; // perform the operations of each route package

    void execute_cycle(std::shared_ptr<Router> router);  // perform the operations of each route package
    bool is_completed();
};

class CopyDataPackage: public RoutePackage {
public:
    CopyDataPackage(int src_pe_row, int src_pe_col, int dst_pe_row, int dst_pe_col, int reg_idx, VectorData vector_data, std::shared_ptr<Inst> inst);
    void operate(std::shared_ptr<Router> router) ; 

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
