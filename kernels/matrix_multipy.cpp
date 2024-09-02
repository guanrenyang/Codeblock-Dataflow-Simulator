#include "DataFlowGraph.h"
#include "PEArray.h"

#include <memory>
#include <vector>

const int PE_ROWS = 4;
const int NUM_PE = PE_ROWS * PE_ROWS;
inline int pe_idx(int row, int col){ return row * PE_ROWS + col; }
const int REG_WIDTH_IN_BYTES = 128;

std::shared_ptr<DataFlowGraph> matrix_multiply(PEArray &pe_array, int A_rows, int A_cols, int B_cols) {
    std::shared_ptr<DataFlowGraph> dfg = std::make_shared<DataFlowGraph>();

    std::vector<std::shared_ptr<CodeBlock>> load_A(NUM_PE, nullptr);
    std::vector<std::shared_ptr<CodeBlock>> load_B(NUM_PE, nullptr);
    std::vector<std::shared_ptr<CodeBlock>> compute(NUM_PE, nullptr);
    std::vector<std::shared_ptr<CodeBlock>> store_result(NUM_PE, nullptr);

    for (int i = 0; i < NUM_PE; ++i) {
        load_A[i] = dfg->createCodeBlock();
        load_B[i] = dfg->createCodeBlock();
        compute[i] = dfg->createDependentCodeBlock(load_A[i]);
        dfg->connectCodeBlock(load_B[i], compute[i]);
        store_result[i] = dfg->createDependentCodeBlock(compute[i]);
    }

    for (int pe_row = 0; pe_row < PE_ROWS; pe_row++) {
        for (int pe_col = 0; pe_col < PE_ROWS; pe_col++) {
            pe_array.add_CodeBlock(pe_row, pe_col, load_A[pe_idx(pe_row, pe_col)]);
            pe_array.add_CodeBlock(pe_row, pe_col, load_B[pe_idx(pe_row, pe_col)]);
            pe_array.add_CodeBlock(pe_row, pe_col, compute[pe_idx(pe_row, pe_col)]);
            pe_array.add_CodeBlock(pe_row, pe_col, store_result[pe_idx(pe_row, pe_col)]);
        }
    }

    int total_elements = A_rows * B_cols;
    int num_elements_per_PE = (total_elements + NUM_PE - 1) / NUM_PE; 
    int B_matrix_offset = A_rows * A_cols * REG_WIDTH_IN_BYTES;

    for (int pe_row = 0; pe_row < PE_ROWS; ++pe_row) {
        for (int pe_col = 0; pe_col < PE_ROWS; ++pe_col) {
            int idx = pe_idx(pe_row, pe_col);

            int num_elements_per_PE = (A_rows * B_cols + NUM_PE - 1) / NUM_PE;

            int start_global_idx = idx * num_elements_per_PE;
            int end_global_idx = std::min(start_global_idx + num_elements_per_PE, A_rows * B_cols);

            std::set<int> unique_rows;
            for (int global_idx = start_global_idx; global_idx < end_global_idx; ++global_idx) {
                int row = global_idx / B_cols;
                unique_rows.insert(row);
            }
            int reg_offset = 0;
            for (int row : unique_rows) {
                for (int k = 0; k < A_cols; ++k) {
                    int spm_offset_A = (row * A_cols + k) * REG_WIDTH_IN_BYTES;
                    dfg->appendLoad(load_A[idx], pe_row, pe_col, reg_offset, spm_offset_A);
                    ++reg_offset;
                }
            }

            int B_reg_offset = reg_offset;
            std::set<int> unique_cols;
            for (int global_idx = start_global_idx; global_idx < end_global_idx; ++global_idx) {
                int col = global_idx % B_cols;
                unique_cols.insert(col);
            }
            for (int col : unique_cols) {
                for (int k = 0; k < A_cols; ++k) {
                    int spm_offset_B = B_matrix_offset + (k * B_cols + col) * REG_WIDTH_IN_BYTES;
                    dfg->appendLoad(load_B[idx], pe_row, pe_col, B_reg_offset, spm_offset_B);
                    ++B_reg_offset;
                }
            }

            int spm_result_start = (A_rows * A_cols + A_cols * B_cols) * REG_WIDTH_IN_BYTES;

            for (int global_idx = start_global_idx; global_idx < end_global_idx; ++global_idx) {
                int row = global_idx / B_cols;
                int col = global_idx % B_cols;
                int reslut_reg_offset = B_reg_offset;

                int A_data_reg_offset = ((global_idx - start_global_idx) / B_cols) * A_cols;
                int B_data_reg_offset = ((global_idx - start_global_idx) % B_cols) * A_cols;

                // Compute
                for (int k = 0; k < A_cols; ++k) {
                    dfg->appendCal(compute[idx], 2, reslut_reg_offset, A_data_reg_offset + k, B_data_reg_offset + k);
                    reslut_reg_offset++;
                }

                reslut_reg_offset = B_reg_offset;
                for (int k = 1; k < A_cols; ++k) {
                    dfg->appendCal(compute[idx], 0, reslut_reg_offset, reslut_reg_offset + k, reslut_reg_offset);
                }

                // Store result
                int spm_store_addr = (row * B_cols + col) * REG_WIDTH_IN_BYTES;
                dfg->appendStore(store_result[idx], pe_row, pe_col, reslut_reg_offset, spm_store_addr);
            }
        }
    }

    return dfg;
}