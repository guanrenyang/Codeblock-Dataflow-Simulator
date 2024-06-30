#include "RoutePackage.h"
#include "Inst.h"
#include "Router.h"
#include "LocalScheduler.h"
#include "SPM.h"

RoutePackage::RoutePackage(std::shared_ptr<Inst> calling_inst) : inst(calling_inst) {}
void RoutePackage::execute_cycle(std::shared_ptr<Router> router) { // perform the operations of each route package
    if (remaining_hops > 0) {
        remaining_hops--;
    } else {
        operate(router);
        completed = true;
    }
} 
bool RoutePackage::is_completed(){
    return completed;
}

CopyDataPackage::CopyDataPackage(int src_pe_row, int src_pe_col, int dst_pe_row, int dst_pe_col, int reg_idx, VectorData vector_data, std::shared_ptr<Inst> inst) : RoutePackage(inst) {
    this->dst_pe_row_idx = dst_pe_row;
    this->dst_pe_col_idx = dst_pe_col;
    this->reg_idx = reg_idx;
    this->data = vector_data;
    this->remaining_hops = std::abs(src_pe_row - dst_pe_row) + std::abs(src_pe_col - dst_pe_col);
}

void CopyDataPackage::operate(std::shared_ptr<Router> router) {
    router->write_pe_reg(dst_pe_row_idx, dst_pe_col_idx, 0, data);
    this->inst->remove_async_inst();
} 

LoadSignalPackage::LoadSignalPackage(int dst_pe_row, int dst_pe_col, int dst_reg_idx, uint32_t addr, std::shared_ptr<Inst> inst) : RoutePackage(inst) {
    this->dst_pe_row_idx = dst_pe_row;
    this->dst_pe_col_idx = dst_pe_col;
    this->reg_idx = dst_reg_idx;
    this->spm_addr = addr;
    // suppose SPM is on the top left of PEarrays, and it needs s1 hop to PE(0,0)
    this->remaining_hops = std::abs(0 - dst_pe_row) + std::abs(0 - dst_pe_col) + 1;
}

void LoadSignalPackage::operate(std::shared_ptr<Router> router) {
    VectorData res =  router->read_shared_spm(this->spm_addr);
    std::shared_ptr<RoutePackage> load_data_package = std::make_shared<LoadDataPackage>(this->dst_pe_row_idx, this->dst_pe_col_idx, this->reg_idx, res, inst);
    router->put(load_data_package);
} 

LoadDataPackage::LoadDataPackage(int dst_pe_row, int dst_pe_col, int dst_reg_idx, VectorData dst_data, std::shared_ptr<Inst> inst) : RoutePackage(inst) {
    this->dst_pe_row_idx = dst_pe_row;
    this->dst_pe_col_idx = dst_pe_col;
    this->reg_idx = dst_reg_idx;
    this->data = dst_data;
    // suppose SPM is on the top left of PEarrays, and it needs 1 hop to PE(0,0)
    this->remaining_hops = std::abs(0 - dst_pe_row) + std::abs(0 - dst_pe_col) + 1;
}

void LoadDataPackage::operate(std::shared_ptr<Router> router) {
    router->write_pe_reg(this->dst_pe_row_idx, this->dst_pe_col_idx, this->reg_idx, this->data);
    this->inst->remove_async_inst();
}

StoreDataPackage::StoreDataPackage(int src_pe_row, int src_pe_col, uint32_t addr, VectorData src_data, std::shared_ptr<Inst> inst) : RoutePackage(inst){
    this->spm_addr = addr;
    this->data = src_data;
    this->remaining_hops = std::abs(0 - src_pe_row) + std::abs(0 - src_pe_col) + 1;
}

void StoreDataPackage::operate(std::shared_ptr<Router> router) {
    router->write_shared_spm(this->spm_addr, this->data);
    std::shared_ptr<RoutePackage> store_ack_signal = std::make_shared<StoreAckPackage>(inst);
    router->put(store_ack_signal);
}


StoreAckPackage::StoreAckPackage(std::shared_ptr<Inst> inst): RoutePackage(inst) {
    this->inst = inst;
    this->remaining_hops = 0;
}


void StoreAckPackage::operate(std::shared_ptr<Router> router)
{
    this->inst->remove_async_inst();
}
