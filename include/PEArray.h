#ifndef PEARRAY_H
#define PEARRAY_H

#include "Router.h"
#include "common.h"
#include "LocalScheduler.h"
#include "PE.h"
#include "SPM.h"
#include <memory>


class PEArray {
    /* The PEArray class manages PEs and On-Chip routers */
public:
    PEArray(int _num_row, int _num_col, int _memory_size) : num_row(_num_row), num_col(_num_col){
        // initialize PE array
        for (int i = 0; i < _num_row; ++i) {
            std::vector<PE> row;
            for (int j = 0; j < _num_col; ++j) {
                row.emplace_back(PE());
            }
            PE_array_2d.push_back(row);
        }
        // initialize Register File
        for (int i = 0; i < _num_row; ++i) {
            std::vector<std::shared_ptr<VectorRegisterFile>> row;
            for (int j = 0; j < _num_col; ++j) {
                auto reg_file = std::make_shared<VectorRegisterFile>();
                (*reg_file).resize(2048); 
                row.push_back(reg_file);
            }
            reg_array_2d.push_back(row);
        }

        // initialize SPM
        spm = std::make_shared<SPM>(_memory_size);
        router = std::make_shared<Router>([this](int pe_row, int pe_col, int reg_idx, VectorData new_data){
            this->write_pe_reg(pe_row, pe_col, reg_idx, new_data);
        });

        // attach memory to each PE
        for (int i = 0; i < _num_row; i++) {
            for (int j = 0; j < _num_col; j++) {
                PE_array_2d[i][j].set_memory(spm);
                PE_array_2d[i][j].set_register_file(reg_array_2d[i][j]);
                PE_array_2d[i][j].set_router(router);
            }
        }
    }

    void execute_cycle(){
        for (int i = 0; i < num_row; i++) {
            for (int j = 0; j < num_col; j++) {
                PE_array_2d[i][j].execute_cycle();
            }
        } 

        // execute router
        router->execute_cycle();
    }

    void add_CodeBlock(int pe_row, int pe_col, std::shared_ptr<CodeBlock> code_block){
        std::cout<<"add code block to PE["<<pe_row<<"]["<<pe_col<<"]\n";
        assert(pe_row < num_row && pe_col < num_col);
        PE_array_2d[pe_row][pe_col].add_CodeBlock(code_block);
    }

    void write_pe_reg(int pe_row, int pe_col, int reg_idx, const VectorData& new_data){
        (*reg_array_2d[pe_row][pe_col])[reg_idx].write_reg(new_data);
    }

    void display_reg(int pe_row, int pe_col, int reg_idx){
        PE_array_2d[pe_row][pe_col].display_reg_as_fp32(reg_idx);
    }

private:
    int num_row, num_col;
    std::vector<std::vector<PE>> PE_array_2d;
    std::vector<std::vector<std::shared_ptr<VectorRegisterFile>>> reg_array_2d;
    std::shared_ptr<SPM> spm;
    std::shared_ptr<Router> router;
};
#endif