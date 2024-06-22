#include "RoutePackage.h"
#include "Inst.h"
#include "Router.h"
#include "LocalScheduler.h"

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
