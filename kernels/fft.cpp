#include <memory>

#include "DataFlowGraph.h"
#include "PEArray.h"

const int PE_ROWS = 4;
const int NUM_PE = PE_ROWS * PE_ROWS;
inline int pe_idx(int row, int col) { return row * PE_ROWS + col; }
const int NUM_IN_USE_REG_PER_PE = 2;
const int REG_WIDTH_IN_BYTES = 128; // Bytes
inline void get_pe_row_col(int idx, int &row, int &col) {
    row = idx / PE_ROWS;
    col = idx % PE_ROWS;
}

std::array<uint32_t, 32> shuffle_data = {
    0, 16, 8, 24, 4, 20, 12, 28, 2, 18, 10, 26, 6, 22, 14, 30,
    1, 17, 9, 25, 5, 21, 13, 29, 3, 19, 11, 27, 7, 23, 15, 31};

int get_pair_id(int n, int interval) {
    int res;
    if ((n / interval) % 2)
        res = n - interval;
    else
        res = n + interval;
    return res;
}

void process_stage(std::shared_ptr<DataFlowGraph> &dfg,
                   std::vector<std::shared_ptr<CodeBlock>> &prev_stage,
                   std::vector<std::shared_ptr<CodeBlock>> &curr_stage,
                   int interval) {
    for (int pe_idx = 0; pe_idx < NUM_PE; pe_idx++) {
        for (int i = 0; i < NUM_IN_USE_REG_PER_PE; i++) {
            curr_stage[pe_idx * NUM_IN_USE_REG_PER_PE + i] =
                dfg->createDependentCodeBlock(
                    prev_stage[pe_idx * NUM_IN_USE_REG_PER_PE + i]);
            int pair_pe_idx = get_pair_id(pe_idx, interval);
            dfg->connectCodeBlock(
                prev_stage[pair_pe_idx * NUM_IN_USE_REG_PER_PE + i],
                curr_stage[pe_idx * NUM_IN_USE_REG_PER_PE + i]);
        }
    }
}


void compute_twiddle_factors(int k, int N, VectorData &real_data,
                             VectorData &imag_data) {
    // compute_twiddle_factors
    float angle = -2.0f * M_PI * k / N;
    float real_part = cos(angle);
    float imag_part = sin(angle);

    int num_elements = real_data.size() / sizeof(float);
    float *real_ptr = reinterpret_cast<float *>(real_data.data());
    float *imag_ptr = reinterpret_cast<float *>(imag_data.data());

    for (int i = 0; i < num_elements; ++i) {
        real_ptr[i] = real_part;
        imag_ptr[i] = imag_part;
    }
}

void butterfly(std::shared_ptr<DataFlowGraph> &dfg,
               std::vector<std::shared_ptr<CodeBlock>> butterfly_compute,
               int interval) {
    for (int pe_row = 0; pe_row < PE_ROWS; pe_row++) {
        for (int pe_col = 0; pe_col < PE_ROWS; pe_col++) {
            int index = pe_idx(pe_row, pe_col);
            int pair_index = get_pair_id(index, interval);

            // 跨越两个PE的蝶形计算，最好是传递一次数据，把两个计算(A+BW,
            // A-BW)放在一个PE上计算 再把其中一个结果传递到另一个PE上
            // 一个PE内有NUM_IN_USE_REG_PER_PE个数据, 应该平均分配到两个PE计算

            int reg_data_start = 0, reg_data_end = NUM_IN_USE_REG_PER_PE / 2;
            if (pair_index < index) {
                reg_data_start = reg_data_end;
                reg_data_end = NUM_IN_USE_REG_PER_PE;
            }

            int pair_pe_row, pair_pe_col;
            get_pe_row_col(pair_index, pair_pe_row, pair_pe_col);

            for (int i = reg_data_start; i < reg_data_end; i++) {
                // 根据 pair_index 和 index 的关系设置数据位置
                int cb_idx = index * NUM_IN_USE_REG_PER_PE + i;
                int real_data_A, imag_data_A, real_data_B, imag_data_B;
                if (pair_index > index) {
                    real_data_A = i * 2;
                    imag_data_A = i * 2 + 1;
                    real_data_B = real_data_A + NUM_IN_USE_REG_PER_PE * 2;
                    imag_data_B = imag_data_A + NUM_IN_USE_REG_PER_PE * 2;
                    dfg->appendCopy(butterfly_compute[cb_idx], pair_pe_row,
                                    pair_pe_col, real_data_A, pe_row, pe_col,
                                    real_data_B);
                    dfg->appendCopy(butterfly_compute[cb_idx], pair_pe_row,
                                    pair_pe_col, imag_data_A, pe_row, pe_col,
                                    imag_data_B);
                } else {
                    real_data_B = i * 2;
                    imag_data_B = i * 2 + 1;
                    real_data_A = real_data_B + NUM_IN_USE_REG_PER_PE * 2;
                    imag_data_A = imag_data_B + NUM_IN_USE_REG_PER_PE * 2;
                    dfg->appendCopy(butterfly_compute[cb_idx], pair_pe_row,
                                    pair_pe_col, real_data_B, pe_row, pe_col,
                                    real_data_A);
                    dfg->appendCopy(butterfly_compute[cb_idx], pair_pe_row,
                                    pair_pe_col, imag_data_B, pe_row, pe_col,
                                    imag_data_A);
                }

                // 获取旋转因子
                VectorData twiddle_real, twiddle_imag;
                int k = i;
                int N = interval * 2; // 根据当前的 interval 计算 N
                compute_twiddle_factors(k, N, twiddle_real, twiddle_imag);
                int twiddle_real_reg = NUM_IN_USE_REG_PER_PE * 2 * 2;
                int twiddle_imag_reg = twiddle_real_reg + 1;
                dfg->appendMovImm(butterfly_compute[cb_idx], twiddle_real_reg,
                                  twiddle_real);
                dfg->appendMovImm(butterfly_compute[cb_idx], twiddle_imag_reg,
                                  twiddle_imag);

                // Butterfly 计算
                int real_mul_real = twiddle_imag_reg + 1,
                    imag_mul_imag = twiddle_imag_reg + 2;
                int real_mul_imag = twiddle_imag_reg + 3,
                    imag_mul_real = twiddle_imag_reg + 4;
                dfg->appendCal(butterfly_compute[cb_idx], 2, real_mul_real,
                               real_data_B, twiddle_real_reg);
                dfg->appendCal(butterfly_compute[cb_idx], 2, imag_mul_imag,
                               imag_data_B, twiddle_imag_reg);
                dfg->appendCal(butterfly_compute[cb_idx], 2, real_mul_imag,
                               real_data_B, twiddle_imag_reg);
                dfg->appendCal(butterfly_compute[cb_idx], 2, imag_mul_real,
                               imag_data_B, twiddle_real_reg);
                int real_data_BW = real_mul_real, imag_data_BW = imag_mul_imag;
                // BW_real = B_real * W_real - B_imag * W_imag
                dfg->appendCal(butterfly_compute[cb_idx], 1, real_data_BW,
                               real_mul_real, imag_mul_imag);
                // BW_imag = B_real * W_imag + B_imag * W_real
                dfg->appendCal(butterfly_compute[cb_idx], 0, imag_data_BW,
                               real_mul_imag, imag_mul_real);

                // A+BW
                int real_A_plus_BW = real_mul_imag,
                    imag_A_plus_BW = imag_mul_real;
                dfg->appendCal(butterfly_compute[cb_idx], 0, real_A_plus_BW,
                               real_data_A, real_data_BW);
                dfg->appendCal(butterfly_compute[cb_idx], 0, imag_A_plus_BW,
                               imag_data_A, imag_data_BW);

                // compute A-BW，可以暂时覆盖掉BW的位置
                int real_A_sub_BW = real_data_BW, imag_A_sub_BW = imag_data_BW;
                dfg->appendCal(butterfly_compute[cb_idx], 1, real_A_sub_BW,
                               real_data_A, real_data_BW);
                dfg->appendCal(butterfly_compute[cb_idx], 1, imag_A_sub_BW,
                               imag_data_A, imag_data_BW);

                // 根据 PE 的相对位置存储 A+BW 和 A-BW
                if (pair_index > index) {
                    // A+BW存放在A的位置， A-BW存放在另一个PE上A的位置
                    dfg->appendMovReg(butterfly_compute[cb_idx], real_A_plus_BW,
                                      real_data_A);
                    dfg->appendMovReg(butterfly_compute[cb_idx], imag_A_plus_BW,
                                      imag_data_A);
                    dfg->appendCopy(butterfly_compute[cb_idx], pe_row, pe_col,
                                    real_A_sub_BW, pair_pe_row, pair_pe_col,
                                    real_data_A);
                    dfg->appendCopy(butterfly_compute[cb_idx], pe_row, pe_col,
                                    imag_A_sub_BW, pair_pe_row, pair_pe_col,
                                    imag_data_A);
                } else {
                    // A-BW存放在B的位置， A+BW存放在另一个PE上B的位置
                    dfg->appendMovReg(butterfly_compute[cb_idx], real_A_sub_BW,
                                      real_data_B);
                    dfg->appendMovReg(butterfly_compute[cb_idx], imag_A_sub_BW,
                                      imag_data_B);
                    dfg->appendCopy(butterfly_compute[cb_idx], pe_row, pe_col,
                                    real_A_plus_BW, pair_pe_row, pair_pe_col,
                                    real_data_B);
                    dfg->appendCopy(butterfly_compute[cb_idx], pe_row, pe_col,
                                    imag_A_plus_BW, pair_pe_row, pair_pe_col,
                                    imag_data_B);
                }
            }
        }
    }
}

std::shared_ptr<DataFlowGraph> fft_1024(PEArray &pe_array) {
    std::shared_ptr<DataFlowGraph> dfg = std::make_shared<DataFlowGraph>();
    /* initialize all the empty code block handlers (Instances of class
     * `CodeBlock` have not been created) */
    std::vector<std::shared_ptr<CodeBlock>> load_and_shuffle(NUM_PE, nullptr);
    std::vector<std::shared_ptr<CodeBlock>> butterfly_compute_1(NUM_PE, nullptr);
    std::vector<std::shared_ptr<CodeBlock>> butterfly_compute_2(
        NUM_PE * NUM_IN_USE_REG_PER_PE, nullptr);
    std::vector<std::shared_ptr<CodeBlock>> butterfly_compute_4(
        NUM_PE * NUM_IN_USE_REG_PER_PE, nullptr);
    std::vector<std::shared_ptr<CodeBlock>> butterfly_compute_8(
        NUM_PE * NUM_IN_USE_REG_PER_PE, nullptr);
    std::vector<std::shared_ptr<CodeBlock>> butterfly_compute_16(
        NUM_PE * NUM_IN_USE_REG_PER_PE, nullptr);
    std::vector<std::shared_ptr<CodeBlock>> store_32(
        NUM_PE * NUM_IN_USE_REG_PER_PE, nullptr);
    std::vector<std::shared_ptr<CodeBlock>> load(NUM_PE * NUM_IN_USE_REG_PER_PE,
                                                 nullptr);
    std::vector<std::shared_ptr<CodeBlock>> butterfly_compute_32(NUM_PE,
                                                                 nullptr);
    std::vector<std::shared_ptr<CodeBlock>> butterfly_compute_64(
        NUM_PE * NUM_IN_USE_REG_PER_PE, nullptr);
    std::vector<std::shared_ptr<CodeBlock>> butterfly_compute_128(
        NUM_PE * NUM_IN_USE_REG_PER_PE, nullptr);
    std::vector<std::shared_ptr<CodeBlock>> butterfly_compute_256(
        NUM_PE * NUM_IN_USE_REG_PER_PE, nullptr);
    std::vector<std::shared_ptr<CodeBlock>> butterfly_compute_512(
        NUM_PE * NUM_IN_USE_REG_PER_PE, nullptr);
    std::vector<std::shared_ptr<CodeBlock>> store_final_result(
        NUM_PE * NUM_IN_USE_REG_PER_PE, nullptr);  

    /* create code blocks and connect them */
    for (auto &cb_ptr : load_and_shuffle) {
        cb_ptr = dfg->createCodeBlock();
    }

    for (int i = 0; i < NUM_PE; i++) {
        butterfly_compute_1[i] =
            dfg->createDependentCodeBlock(load_and_shuffle[i]);
    }

    for (int pe_idx = 0; pe_idx < NUM_PE; pe_idx++) {
        for (int i = 0; i < NUM_IN_USE_REG_PER_PE; i++) {
            butterfly_compute_2[pe_idx * NUM_IN_USE_REG_PER_PE + i] =
                dfg->createDependentCodeBlock(butterfly_compute_1[pe_idx]);
            int pair_pe_idx = get_pair_id(pe_idx, 1);
            dfg->connectCodeBlock(
                butterfly_compute_1[pair_pe_idx],
                butterfly_compute_2[pe_idx * NUM_IN_USE_REG_PER_PE + i]);
        }
    }

    process_stage(dfg, butterfly_compute_2, butterfly_compute_4, 2);
    process_stage(dfg, butterfly_compute_4, butterfly_compute_8, 4);
    process_stage(dfg, butterfly_compute_8, butterfly_compute_16, 8);
    
    for (int pe_idx = 0; pe_idx < NUM_PE; pe_idx++) {
        for (int i = 0; i < NUM_IN_USE_REG_PER_PE; i++) {
            store_32[pe_idx * NUM_IN_USE_REG_PER_PE + i] =
                dfg->createDependentCodeBlock(
                    butterfly_compute_16[pe_idx * NUM_IN_USE_REG_PER_PE + i]);
            load[pe_idx * NUM_IN_USE_REG_PER_PE + i] =
                dfg->createDependentCodeBlock(
                    store_32[pe_idx * NUM_IN_USE_REG_PER_PE + i]);
        }
    }

    for (int pe_idx = 0; pe_idx < NUM_PE; pe_idx++) {
        butterfly_compute_32[pe_idx] = dfg->createCodeBlock();
        for (int i = 0; i < NUM_IN_USE_REG_PER_PE; i++)
            dfg->connectCodeBlock(load[pe_idx * NUM_IN_USE_REG_PER_PE + i],
                                  butterfly_compute_32[pe_idx]);
    }

    for (int pe_idx = 0; pe_idx < NUM_PE; pe_idx++) {
        for (int i = 0; i < NUM_IN_USE_REG_PER_PE; i++) {
            butterfly_compute_64[pe_idx * NUM_IN_USE_REG_PER_PE + i] =
                dfg->createDependentCodeBlock(butterfly_compute_32[pe_idx]);
            int pair_pe_idx = get_pair_id(pe_idx, 1);
            dfg->connectCodeBlock(
                butterfly_compute_32[pair_pe_idx],
                butterfly_compute_64[pe_idx * NUM_IN_USE_REG_PER_PE + i]);
        }
    }

    process_stage(dfg, butterfly_compute_64, butterfly_compute_128, 2);
    process_stage(dfg, butterfly_compute_128, butterfly_compute_256, 4);
    process_stage(dfg, butterfly_compute_256, butterfly_compute_512, 8);
    process_stage(dfg, butterfly_compute_512, store_final_result, 8);

    for (int pe_row = 0; pe_row < PE_ROWS; pe_row++) {
        for (int pe_col = 0; pe_col < PE_ROWS; pe_col++) {
            int index = pe_idx(pe_row, pe_col);
            pe_array.add_CodeBlock(pe_row, pe_col, load_and_shuffle[index]);
            pe_array.add_CodeBlock(pe_row, pe_col, butterfly_compute_1[index]);
            for(int i = 0; i < NUM_IN_USE_REG_PER_PE; i++)
            {
                int j = index * NUM_IN_USE_REG_PER_PE + i;
                pe_array.add_CodeBlock(pe_row, pe_col, butterfly_compute_2[j]);
                pe_array.add_CodeBlock(pe_row, pe_col, butterfly_compute_4[j]);
                pe_array.add_CodeBlock(pe_row, pe_col, butterfly_compute_8[j]);
                pe_array.add_CodeBlock(pe_row, pe_col, butterfly_compute_16[j]);
                pe_array.add_CodeBlock(pe_row, pe_col, store_32[j]);
                pe_array.add_CodeBlock(pe_row, pe_col, load[j]);
            }
            pe_array.add_CodeBlock(pe_row, pe_col, butterfly_compute_32[index]);
            for(int i = 0; i < NUM_IN_USE_REG_PER_PE; i++)
            {
                int j = index * NUM_IN_USE_REG_PER_PE + i;
                pe_array.add_CodeBlock(pe_row, pe_col, butterfly_compute_64[j]);
                pe_array.add_CodeBlock(pe_row, pe_col, butterfly_compute_128[j]);
                pe_array.add_CodeBlock(pe_row, pe_col, butterfly_compute_256[j]);
                pe_array.add_CodeBlock(pe_row, pe_col, butterfly_compute_512[j]);
                pe_array.add_CodeBlock(pe_row, pe_col, store_final_result[j]);
            }
        }
    }
    
    for (int pe_row = 0; pe_row < PE_ROWS; pe_row++) {
        for (int pe_col = 0; pe_col < PE_ROWS; pe_col++) {
            int index = pe_idx(pe_row, pe_col);
            for (int reg_id = 0; reg_id < NUM_IN_USE_REG_PER_PE; reg_id++) {
                int reg_global_id = index * NUM_IN_USE_REG_PER_PE + reg_id;
                int shuffled_address = shuffle_data[reg_global_id];
                int spm_address = shuffled_address * REG_WIDTH_IN_BYTES;
                // dst_reg_id is reg_id * 2 because it is loaded as a real
                // number but the imaginary part needs to be preserved for later
                // calculations
                dfg->appendLoad(load_and_shuffle[index], pe_row, pe_col,
                              reg_id * 2, spm_address);
                VectorData shuffle_mask;
                std::memcpy(shuffle_mask.data(), shuffle_data.data(),
                            shuffle_mask.size());
                
                dfg->appendMovImm(load_and_shuffle[index],
                            NUM_IN_USE_REG_PER_PE * 2, shuffle_mask);
                
                for (int reg_id = 0; reg_id < NUM_IN_USE_REG_PER_PE; reg_id++) {
                    dfg->appendCal(load_and_shuffle[index], 9, reg_id * 2, reg_id *
                2, NUM_IN_USE_REG_PER_PE * 2);
                }
            }
        }
    }

    for (int pe_row = 0; pe_row < PE_ROWS; pe_row++) {
        for (int pe_col = 0; pe_col < PE_ROWS; pe_col++) {
            int index = pe_idx(pe_row, pe_col);
            // A : | reg0(real number) | reg1(imag number) |
            // B : | reg2(real number) | reg3(imag number) |
            // Butterfly operation: C =  A + W * B,  D = A - W * B
            // W == 1 for the first-stage butterfly calculation, so it can be
            // neglected
            //  | reg4 | reg5 |=  A + B
            dfg->appendCal(butterfly_compute_1[index], 0, 4, 0, 2);
            dfg->appendCal(butterfly_compute_1[index], 0, 5, 1, 3);
            // | reg2 | reg3 |=  A - B
            dfg->appendCal(butterfly_compute_1[index], 1, 2, 0, 2);
            dfg->appendCal(butterfly_compute_1[index], 1, 3, 1, 3);    

            // | reg0 | reg1 |=  | reg4 | reg5 | (A + B)
            dfg->appendMovReg(butterfly_compute_1[index], 0, 4);
            dfg->appendMovReg(butterfly_compute_1[index], 1, 5);
        }
    }

    butterfly(dfg, butterfly_compute_2, 1);
    butterfly(dfg, butterfly_compute_4, 2);
    butterfly(dfg, butterfly_compute_8, 4);
    butterfly(dfg, butterfly_compute_16, 8);

        // store32
    int real_address = 0,
        imag_address = NUM_PE * NUM_IN_USE_REG_PER_PE * REG_WIDTH_IN_BYTES;
    int interval = REG_WIDTH_IN_BYTES;
    int step = 1;
    for (int pe_row = 0; pe_row < PE_ROWS; pe_row++) {
        for (int pe_col = 0; pe_col < PE_ROWS; pe_col++) {
            int index = pe_idx(pe_row, pe_col);
            // 先存实数，再存虚数
            for (int i = 0; i < NUM_IN_USE_REG_PER_PE; i++) {
                int cb_idx = index * NUM_IN_USE_REG_PER_PE + i;
                dfg->appendStore(store_32[cb_idx], pe_row, pe_col, i * 2,
                                 real_address, interval);
                real_address += step;
                dfg->appendStore(store_32[cb_idx], pe_row, pe_col, i * 2 + 1,
                                 imag_address, interval);
                imag_address += step;
            }
        }
    }
    // load
    step = REG_WIDTH_IN_BYTES;
    real_address = 0,
    imag_address = NUM_PE * NUM_IN_USE_REG_PER_PE * REG_WIDTH_IN_BYTES;
    for (int pe_row = 0; pe_row < PE_ROWS; pe_row++) {
        for (int pe_col = 0; pe_col < PE_ROWS; pe_col++) {
            int index = pe_idx(pe_row, pe_col);
            // 先laod实数，再load虚数
            for (int i = 0; i < NUM_IN_USE_REG_PER_PE; i++) {
                int cb_idx = index * NUM_IN_USE_REG_PER_PE + i;
                dfg->appendLoad(load[cb_idx], pe_row, pe_col, i * 2,
                                real_address);
                real_address += step;
                dfg->appendLoad(load[cb_idx], pe_row, pe_col, i * 2 + 1,
                                imag_address);
                imag_address += step;
            }
        }
    }

    // butterfly_compute_32
    for (int pe_row = 0; pe_row < PE_ROWS; pe_row++) {
        for (int pe_col = 0; pe_col < PE_ROWS; pe_col++) {
            int index = pe_idx(pe_row, pe_col);
            // A : | reg0(real number) | reg1(imag number) |
            // B : | reg2(real number) | reg3(imag number) |
            // Butterfly operation: C =  A + W * B,  D = A - W * B
            // W == 1 for the first-stage butterfly calculation, so it can be
            // neglected
            //  | reg4 | reg5 |=  A + B
            dfg->appendCal(butterfly_compute_1[index], 0, 4, 0, 2);
            dfg->appendCal(butterfly_compute_1[index], 0, 5, 1, 3);
            // | reg2 | reg3 |=  A - B
            dfg->appendCal(butterfly_compute_1[index], 1, 2, 0, 2);
            dfg->appendCal(butterfly_compute_1[index], 1, 3, 1, 3);

            // | reg0 | reg1 |=  | reg4 | reg5 | (A + B)
            dfg->appendMovReg(butterfly_compute_1[index], 0, 4);
            dfg->appendMovReg(butterfly_compute_1[index], 1, 5);
        }
    }
    
    butterfly(dfg, butterfly_compute_64, 1);
    butterfly(dfg, butterfly_compute_128, 2);
    butterfly(dfg, butterfly_compute_256, 4);
    butterfly(dfg, butterfly_compute_512, 8);

    // store_final_result
    step = REG_WIDTH_IN_BYTES;
    // 可以覆盖之前的地址
    int store_address = 0;
    for (int pe_row = 0; pe_row < PE_ROWS; pe_row++) {
        for (int pe_col = 0; pe_col < PE_ROWS; pe_col++) {
            int index = pe_idx(pe_row, pe_col);
            // 一个数字的实数和虚数放在一起
            for (int i = 0; i < NUM_IN_USE_REG_PER_PE; i++) {
                int cb_idx = index * NUM_IN_USE_REG_PER_PE + i;
                dfg->appendStore(store_final_result[cb_idx], pe_row, pe_col,
                                i * 2, store_address);
                store_address += step;
                dfg->appendStore(store_final_result[cb_idx], pe_row, pe_col,
                                i * 2 + 1, store_address);
                store_address += step;
            }
        }
    }
    return dfg;
}