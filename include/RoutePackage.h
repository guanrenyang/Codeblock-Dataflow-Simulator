#ifndef DATAFLOW_SIMULATOR_ROUTEPACKAGE_H
#define DATAFLOW_SIMULATOR_ROUTEPACKAGE_H

#include "common.h"

class Inst;
class Router;
class SPM;

typedef std::array<uint8_t, 128> VectorData;

class RoutePackage {
private:
    bool completed = false;
public:
    std::shared_ptr<Inst> inst; // source inst
    int remaining_hops;
    RoutePackage(std::shared_ptr<Inst> calling_inst) ;

    virtual void operate(std::shared_ptr<Router> router) = 0; // perform the operations of each route package
    void execute_cycle(std::shared_ptr<Router> router);  // perform the operations of each route package
    bool is_completed();
};

class CopyDataPackage final : public RoutePackage {
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
