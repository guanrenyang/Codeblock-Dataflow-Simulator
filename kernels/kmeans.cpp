#include "DataFlowGraph.h"
#include "PEArray.h"
#include <memory>
#include "Opcode.h"
#include <cmath>

#define points 2
#define center1 0
#define center2 1
#define mask1 3
#define mask2 4
#define dis1 5
#define dis2 6
#define temp1 10
#define temp2 11

const int PE_ROWS = 4;
const int NUM_PE = PE_ROWS * PE_ROWS;
inline int pe_idx(int row, int col){ return row * PE_ROWS + col; }
const int REG_WIDTH_IN_BYTES = 128; // Bytes
//reg0,1:center1,2 , reg2:points, reg3,reg4:mask1,mask2, reg5,reg6:dis1,dis2
//temp reg: 10, 11
//need 3 regs from spm: points, center1,2
const int NUM_IN_USE_REG_PER_PE = 1;

const int RAW_DATA_BYTES = NUM_PE * NUM_IN_USE_REG_PER_PE * REG_WIDTH_IN_BYTES;

float squared_distance(float a, float b) {
    return abs(a - b);
}

// Function to perform K-means clustering on 1D data
std::vector<float> kmeans_1d(int k = 2, int max_iterations = 1) {
    std::vector<float> data(32*16);
    int n = data.size();

    for (int i = 0; i < 512; i++) {
        data[i] = i;
    }

    // Step 1: Initialize k random centroids
    std::vector<float> centroids(k);
    centroids[0] = 1;
    centroids[1] = 2;

    // Step 2: Create clusters vector to hold the cluster assignments
    std::vector<int> clusters(n);

    // Step 3: Iteratively assign points to nearest centroid and update centroids
    for (int iter = 0; iter < max_iterations; ++iter) {
        // Clear clusters
        for (int i = 0; i < n; ++i) {
            clusters[i] = 0;
        }

        // Assign each point to nearest centroid
        for (int i = 0; i < n; ++i) {
            float min_dist = 1000.0;
            int closest_centroid = 0;

            // Find closest centroid
            for (int j = 0; j < k; ++j) {
                float dist = squared_distance(data[i], centroids[j]);
                if (dist < min_dist) {
                    min_dist = dist;
                    closest_centroid = j;
                }
            }

            // Assign point to cluster
            clusters[i] = closest_centroid;
        }

        // Update centroids
        std::vector<float> new_centroids(k);
        std::vector<int> cluster_count(k, 0);

        for (int i = 0; i < n; ++i) {
            new_centroids[clusters[i]] += data[i];
            cluster_count[clusters[i]]++;
        }
        for (int j = 0; j < k; ++j) {
            if (cluster_count[j] > 0) {
                new_centroids[j] /= cluster_count[j];
            }
        }

        // Update centroids
        centroids = new_centroids;
    }

    return centroids;
}


int display(PEArray & pe_array, int cycle) {
    
    if (cycle != 0)
        std::cout << "After cycle " << cycle << " :\n";
    else
        std::cout << " Original :\n";

    std::cout << "RAW_DATA Registers:\n";
    for (int pe_row = 0; pe_row < 4; ++pe_row) {
        for (int pe_col = 0; pe_col < 4; ++pe_col) {
            pe_array.display_reg(pe_row, pe_col, center1);
            pe_array.display_reg(pe_row, pe_col, center2);
            pe_array.display_reg(pe_row, pe_col, points);
            pe_array.display_reg(pe_row, pe_col, dis1);
            pe_array.display_reg(pe_row, pe_col, dis2);
            pe_array.display_reg(pe_row, pe_col, mask1);
            pe_array.display_reg(pe_row, pe_col, mask2);

            
            // pe_array.display_reg(pe_row, pe_col, dis1);
            // pe_array.display_reg(pe_row, pe_col, mask1);
            // pe_array.display_reg(pe_row, pe_col, dis2);
            // pe_array.display_reg(pe_row, pe_col, mask2);
            
        }
    }

    // std::cout << "Intermediate Registers:\n";
    // for (int reg_id = 2+0; reg_id < 2+16; ++reg_id) {
    //     pe_array.display_reg(0, 0, reg_id);
    // }
    // pe_array.display_spm(0);
    // pe_array.display_spm(128);
    // pe_array.display_spm(256);
    // std::cout << std::endl;

    std::vector<float> centroids = kmeans_1d(2, 1);
        // Print centroids
    std::cout << "Centroids:" << std::endl;
    for (int i = 0; i < 2; ++i) {
        std::cout << centroids[i] << " ";
    }
    std::cout << std::endl;
    return 0;
}

std::shared_ptr<DataFlowGraph> kmeans(PEArray & pe_array){
    std::shared_ptr<DataFlowGraph> dfg = std::make_shared<DataFlowGraph>();

    /* initialize all the empty code block handlers (Instances of class `CodeBlock` have not been created) */
    std::vector<std::shared_ptr<CodeBlock>> load_and_cal(NUM_PE, nullptr);
    std::vector<std::shared_ptr<CodeBlock>> copy_all(NUM_PE, nullptr);
    std::shared_ptr<CodeBlock> reduction = nullptr;

    // std::vector<std::shared_ptr<CodeBlock>> copy_down_row_1(NUM_PE, nullptr);
    // std::vector<std::shared_ptr<CodeBlock>> copy_up_row_4(NUM_PE, nullptr);
    // std::vector<std::shared_ptr<CodeBlock>> cal_row_2(NUM_PE, nullptr);
    // std::vector<std::shared_ptr<CodeBlock>> cal_row_3(NUM_PE, nullptr);
    // std::shared_ptr<CodeBlock> cal_new_centers = nullptr;


    /* create code blocks and connect them */
    // load CodeBlocks
    for (auto & cb_ptr: load_and_cal){
        cb_ptr = dfg->createCodeBlock();
    }
    reduction = dfg->createDependentCodeBlock(load_and_cal[0]);
    for (int i = 1; i < NUM_PE; i++) {
        copy_all[i] = dfg->createDependentCodeBlock(load_and_cal[i]);
        dfg->connectCodeBlock(copy_all[i], reduction);
    }

    

    // for (int pe_col = 0; pe_col < PE_ROWS; pe_col++) {
    //     int pe_row = 0;
    //     copy_down_row_1[pe_idx(pe_row, pe_col)] = dfg->createDependentCodeBlock(load_and_cal[pe_idx(pe_row, pe_col)]);
    //     pe_row = 3;
    //     copy_up_row_4[pe_idx(pe_row, pe_col)] = dfg->createDependentCodeBlock(load_and_cal[pe_idx(pe_row, pe_col)]);
    // }

    // for (int pe_col = 0; pe_col < PE_ROWS; pe_col++) {
    //     int pe_row = 1;
    //     cal_row_2[pe_idx(pe_row, pe_col)] = dfg->createDependentCodeBlock(load_and_cal[pe_idx(pe_row, pe_col)]);
    //     dfg->connectCodeBlock(copy_down_row_1[pe_idx(pe_row - 1, pe_col)], cal_row_2[pe_idx(pe_row, pe_col)]);

    //     pe_row = 2;
    //     cal_row_3[pe_idx(pe_row, pe_col)] = dfg->createDependentCodeBlock(load_and_cal[pe_idx(pe_row, pe_col)]);
    //     dfg->connectCodeBlock(copy_up_row_4[pe_idx(pe_row + 1, pe_col)], cal_row_3[pe_idx(pe_row, pe_col)]);
    // }



    /* bind CodeBlocks to PEArray */
    for (int pe_row = 0; pe_row < PE_ROWS; pe_row++) {
        for (int pe_col = 0; pe_col < PE_ROWS; pe_col++) {
            pe_array.add_CodeBlock(pe_row, pe_col, load_and_cal[pe_idx(pe_row, pe_col)]);
            if (pe_row != 0 || pe_col != 0)
                pe_array.add_CodeBlock(pe_row, pe_col, copy_all[pe_idx(pe_row, pe_col)]);
        }
    }
    pe_array.add_CodeBlock(0, 0, reduction);




    /* append instructions to the code blocks */
    for (int pe_row = 0;pe_row < PE_ROWS;pe_row++) {
        for (int pe_col = 0; pe_col < PE_ROWS; pe_col++) {
            // load, first 2 spm addr is center addr
            int spm_start_per_pe = (pe_idx(pe_row, pe_col) * NUM_IN_USE_REG_PER_PE + 2) * REG_WIDTH_IN_BYTES;

            dfg->appendLoad(load_and_cal[pe_idx(pe_row, pe_col)], pe_row, pe_col, center1, 0);
            dfg->appendLoad(load_and_cal[pe_idx(pe_row, pe_col)], pe_row, pe_col, center2, REG_WIDTH_IN_BYTES);
            dfg->appendLoad(load_and_cal[pe_idx(pe_row, pe_col)], pe_row, pe_col, points, spm_start_per_pe);


            //sub to get the distance 1
            dfg->appendCal(load_and_cal[pe_idx(pe_row, pe_col)], Sub_vv, temp1, center1, points);
            dfg->appendCal(load_and_cal[pe_idx(pe_row, pe_col)], Sub_vv, temp2, points, center1);
            //get the abs distance 1
            dfg->appendCal(load_and_cal[pe_idx(pe_row, pe_col)], Abs_vv, temp1, temp1, 0);
            dfg->appendCal(load_and_cal[pe_idx(pe_row, pe_col)], Abs_vv, temp2, temp2, 0);
            //get the distance 1
            dfg->appendCal(load_and_cal[pe_idx(pe_row, pe_col)], Max_vv, dis1, temp1, temp2);

            //the same to distance2
            //sub to get the distance 2
            dfg->appendCal(load_and_cal[pe_idx(pe_row, pe_col)], Sub_vv, temp1, center2, points);
            dfg->appendCal(load_and_cal[pe_idx(pe_row, pe_col)], Sub_vv, temp2, points, center2);
            //get the abs distance 
            dfg->appendCal(load_and_cal[pe_idx(pe_row, pe_col)], Abs_vv, temp1, temp1, 0);
            dfg->appendCal(load_and_cal[pe_idx(pe_row, pe_col)], Abs_vv, temp2, temp2, 0);
            //get the distance 2
            dfg->appendCal(load_and_cal[pe_idx(pe_row, pe_col)], Max_vv, dis2, temp1, temp2);

            //get the mask1, mask2
            dfg->appendCal(load_and_cal[pe_idx(pe_row, pe_col)], Max_mask_vv, mask1, dis1, dis2);
            dfg->appendCal(load_and_cal[pe_idx(pe_row, pe_col)], Max_mask_vv, mask2, dis2, dis1);

            //get the valid distance of center1, center2
            dfg->appendCal(load_and_cal[pe_idx(pe_row, pe_col)], Mul_vv, dis1, mask1, points);
            dfg->appendCal(load_and_cal[pe_idx(pe_row, pe_col)], Mul_vv, dis2, mask2, points);
            //get sum
            dfg->appendCal(load_and_cal[pe_idx(pe_row, pe_col)], V_add, dis1, dis1, 0);
            dfg->appendCal(load_and_cal[pe_idx(pe_row, pe_col)], V_add, dis2, dis2, 0);
            dfg->appendCal(load_and_cal[pe_idx(pe_row, pe_col)], V_add, mask1, mask1, 0);
            dfg->appendCal(load_and_cal[pe_idx(pe_row, pe_col)], V_add, mask2, mask2, 0);
        }
    }

    for (int pe_row = 0, i = temp1; pe_row < PE_ROWS; pe_row++) {
        for (int pe_col = 0; pe_col < PE_ROWS; pe_col++) {
            if (pe_row != 0 || pe_col != 0) {
                dfg->appendCopy(copy_all[pe_idx(pe_row, pe_col)], pe_row, pe_col, dis1, 0, 0, i++);
                dfg->appendCopy(copy_all[pe_idx(pe_row, pe_col)], pe_row, pe_col, mask1, 0, 0, i++);
                dfg->appendCopy(copy_all[pe_idx(pe_row, pe_col)], pe_row, pe_col, dis2, 0, 0, i++);
                dfg->appendCopy(copy_all[pe_idx(pe_row, pe_col)], pe_row, pe_col, mask2, 0, 0, i++);
            }
        }
    }

    // reduction
    int reg_idx_start = temp1;
    for (int reg_id = temp1; reg_id < temp1 + 4 * (NUM_PE - 1); reg_id += 4) {
        dfg->appendCal(reduction, Add_vv, dis1, dis1, reg_id);
        dfg->appendCal(reduction, Add_vv, mask1, mask1, reg_id + 1);
        dfg->appendCal(reduction, Add_vv, dis2, dis2, reg_id + 2);
        dfg->appendCal(reduction, Add_vv, mask2, mask2, reg_id + 3);
    }
    dfg->appendCal(reduction, Div_vv, center1, dis1, mask1);
    dfg->appendCal(reduction, Div_vv, center2, dis2, mask2);

    return dfg;
}
