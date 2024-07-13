#include "DataFlowGraph.h"
#include "PEArray.h"

#include <memory>

const int PE_ROWS = 4;
const int NUM_PE = PE_ROWS * PE_ROWS;
inline int pe_idx(int row, int col){ return row * PE_ROWS + col; }
const int REG_WIDTH_IN_BYTES = 128; // Bytes
const int NUM_IN_USE_REG_PER_PE = 2;

const int RAW_DATA_BYTES = NUM_PE * NUM_IN_USE_REG_PER_PE * REG_WIDTH_IN_BYTES;

int display(PEArray & pe_array, int cycle) {
    if (cycle != 0)
        std::cout << "After cycle " << cycle << " :\n";
    else
        std::cout << " Original :\n";

    std::cout << "RAW_DATA Registers:\n";
    for (int pe_row = 0; pe_row < 4; ++pe_row) {
        for (int pe_col = 0; pe_col < 4; ++pe_col) {
            pe_array.display_reg(pe_row, pe_col, 0);
            pe_array.display_reg(pe_row, pe_col, 1);
        }
    }

    std::cout << "Intermediate Registers:\n";
    for (int reg_id = 2+0; reg_id < 2+16; ++reg_id) {
        pe_array.display_reg(0, 0, reg_id);
    }
    pe_array.display_spm(0);
    pe_array.display_spm(128);
    pe_array.display_spm(256);
    std::cout << std::endl;
    return 0;
}

std::shared_ptr<DataFlowGraph> vector_add(PEArray & pe_array){
    std::shared_ptr<DataFlowGraph> dfg = std::make_shared<DataFlowGraph>();

    /* initialize all the empty code block handlers (Instances of class `CodeBlock` have not been created) */
    std::vector<std::shared_ptr<CodeBlock>> load_and_add(NUM_PE, nullptr);
    std::vector<std::shared_ptr<CodeBlock>> store_intermediate(NUM_PE, nullptr);
    std::shared_ptr<CodeBlock> load_intermediate = nullptr;
    std::shared_ptr<CodeBlock> global_reduce_and_invector_reduce_and_store = nullptr;


    /* create code blocks and connect them */
    // load CodeBlocks
    for (auto & cb_ptr: load_and_add){
        cb_ptr = dfg->createCodeBlock();
    }

    for (int i=0;i<NUM_PE;i++){
        store_intermediate[i] = dfg->createDependentCodeBlock(load_and_add[i]);
    }

    load_intermediate = dfg->createCodeBlock();
    for (int i=0;i<NUM_PE;i++){
        dfg->connectCodeBlock(store_intermediate[i], load_intermediate);
    }

    global_reduce_and_invector_reduce_and_store = dfg->createDependentCodeBlock(load_intermediate);

    /* bind CodeBlocks to PEArray */
    for (int pe_row = 0; pe_row < PE_ROWS; pe_row++) {
        for (int pe_col = 0; pe_col < PE_ROWS; pe_col++) {
            pe_array.add_CodeBlock(pe_row, pe_col, load_and_add[pe_idx(pe_row, pe_col)]);
            pe_array.add_CodeBlock(pe_row, pe_col, store_intermediate[pe_idx(pe_row, pe_col)]);
        }
    }
    pe_array.add_CodeBlock(0, 0, load_intermediate);
    pe_array.add_CodeBlock(0, 0, global_reduce_and_invector_reduce_and_store);

    /* append instructions to the code blocks */
    for (int pe_row = 0;pe_row < PE_ROWS;pe_row++) {
        for (int pe_col = 0; pe_col < PE_ROWS; pe_col++) {
            // load
            int spm_start_per_pe = pe_idx(pe_row, pe_col) * NUM_IN_USE_REG_PER_PE * REG_WIDTH_IN_BYTES;
            for (int reg_id = 0; reg_id < NUM_IN_USE_REG_PER_PE; reg_id++) {
                int spm_offset = reg_id * REG_WIDTH_IN_BYTES;
                dfg->appendLoad(load_and_add[pe_idx(pe_row, pe_col)], pe_row, pe_col, reg_id, spm_start_per_pe + spm_offset);
            }
            for (int reg_id = 1; reg_id < NUM_IN_USE_REG_PER_PE; reg_id++) { // result stored in reg0
                dfg->appendCal(load_and_add[pe_idx(pe_row, pe_col)], 0, 0, 0, reg_id);
            }
        }
    }


    int spm_result_start = RAW_DATA_BYTES;
    // store intermediate
    for (int pe_row = 0;pe_row < PE_ROWS;pe_row++) {
        for (int pe_col = 0; pe_col < PE_ROWS; pe_col++) {
            // store intermediate
            int spm_store_addr_per_pe = pe_idx(pe_row, pe_col) * REG_WIDTH_IN_BYTES;
            dfg->appendStore(store_intermediate[pe_idx(pe_row, pe_col)], pe_row, pe_col, 0, spm_result_start + spm_store_addr_per_pe);
        }
    }
    // load intermediate
    for (int pe_row = 0;pe_row < PE_ROWS; pe_row++) {
        for (int pe_col = 0; pe_col < PE_ROWS; pe_col++) {
            // load intermediate
            int spm_offset_per_pe = pe_idx(pe_row, pe_col) * REG_WIDTH_IN_BYTES;
            for (int reg_id = 0; reg_id < NUM_PE; reg_id++) {
                dfg->appendLoad(load_intermediate, 0, 0, NUM_IN_USE_REG_PER_PE + reg_id, spm_result_start + spm_offset_per_pe);
            }
        }
    }

    // global reduction
    int reg_idx_start = NUM_IN_USE_REG_PER_PE;
    for (int reg_id = reg_idx_start + 1; reg_id < reg_idx_start + NUM_PE; reg_id++) {
        dfg->appendCal(global_reduce_and_invector_reduce_and_store, 0, reg_idx_start, reg_idx_start, reg_id);
    }
    dfg->appendCal(global_reduce_and_invector_reduce_and_store, 8, reg_idx_start, reg_idx_start, reg_idx_start);
    dfg->appendStore(global_reduce_and_invector_reduce_and_store, 0, 0, reg_idx_start, 0);

    return dfg;
}
