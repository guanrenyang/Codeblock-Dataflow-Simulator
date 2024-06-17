#ifndef PEARRAY_H
#define PEARRAY_H

#include "common.h"
#include "LocalScheduler.h"
#include "PE.h"
#include "SPM.h"

template <int _num_row, int _num_col, int _memory_size>
class PEArray {
    /* The PEArray class manages PEs and On-Chip routers */
public:
    PEArray() {
        // make PE array
        spm = std::make_shared<SPM>(_memory_size);
        // attach memory to each PE
        for (int i = 0; i < _num_row; i++) {
            for (int j = 0; j < _num_col; j++) {
                PE_array_2d[i][j].set_memory(spm);
            }
        }
    }

    void execute_cycle(){
        for (int i = 0; i < _num_row; i++) {
            for (int j = 0; j < _num_col; j++) {
                PE_array_2d[i][j].execute_cycle();
            }
        } 
    }

    void add_CodeBlock(int pe_row, int pe_col, std::shared_ptr<CodeBlock> code_block){
        std::cout<<"add code block to PE["<<pe_row<<"]["<<pe_col<<"]\n";
        assert(pe_row < _num_row && pe_col < _num_col);
        PE_array_2d[pe_row][pe_col].add_CodeBlock(code_block);
    }

    void display_reg(int pe_row, int pe_col, int reg_idx){
        PE_array_2d[pe_row][pe_col].display_reg_as_fp32(reg_idx);
    }
private:
    std::array<std::array<PE, _num_col>, _num_col> PE_array_2d;
    std::shared_ptr<SPM> spm;
};
#endif