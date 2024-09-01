#ifndef DATAFLOW_SIMULATOR_ROUTEPACKAGE_H
#define DATAFLOW_SIMULATOR_ROUTEPACKAGE_H

#include "common.h"

class Inst;
class Router;
class SPM;

typedef std::array<uint8_t, 128> VectorData;
typedef std::array<uint8_t, 512> TensorData;

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

class LoadSignalPackage final : public RoutePackage, public std::enable_shared_from_this<LoadSignalPackage> {
    // Read SPM
public:
    LoadSignalPackage(int dst_pe_row, int dst_pe_col, int dst_reg_idx, uint32_t addr, std::shared_ptr<Inst> inst);
    void operate(std::shared_ptr<Router> router) ; 
    int dst_pe_row_idx;
    int dst_pe_col_idx;
    int reg_idx;
    int spm_addr;
};

class LoadDataPackage final : public RoutePackage {
    // Write to reg
public:
    LoadDataPackage(int dst_pe_row, int dst_pe_col, int dst_reg_idx, VectorData dst_data, std::shared_ptr<Inst> inst);
    void operate(std::shared_ptr<Router> router);
    int dst_pe_row_idx;
    int dst_pe_col_idx;
    int reg_idx;
    VectorData data;
};

class StoreDataPackage final : public RoutePackage, public std::enable_shared_from_this<StoreDataPackage>{
    // Write to spm
public:
    StoreDataPackage(int src_pe_row, int src_pe_col, uint32_t addr, VectorData src_data, std::shared_ptr<Inst> inst);
    void operate(std::shared_ptr<Router> router); 
    int pe_row, pe_col;
    uint32_t spm_addr;
    VectorData data;
};

class StoreAckPackage final: public RoutePackage {
public:
    int pe_row, pe_col;
    StoreAckPackage(std::shared_ptr<Inst> inst, int src_pe_row, int src_pe_col);
    void operate(std::shared_ptr<Router> router) ;
};
#endif //DATAFLOW_SIMULATOR_ROUTEPACKAGE_H
