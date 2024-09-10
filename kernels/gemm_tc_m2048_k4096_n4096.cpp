#include "PEArray.h"
#include "DataFlowGraph.h"

const uint32_t DUMMY_ADDRESS = 0xdeadbeef;

const int NUM_K_OUTER = 16;
const int NUM_M_TILE = 8;
const int NUM_N_TILE = 16;
const int NUM_K_TILE = 16; // K dim = NUM_K_TILE * NUM_K_OUTER

const int PE_ROWS = 4;
const int PE_COLS = 4;

static std::vector<std::vector<std::vector<std::shared_ptr<CodeBlock>>>> make_multiDim_CodeBlocks(std::shared_ptr<DataFlowGraph> dfg, int a, int b, int c) {
    return std::vector<std::vector<std::vector<std::shared_ptr<CodeBlock>>>>(a, std::vector<std::vector<std::shared_ptr<CodeBlock>>>(b, std::vector<std::shared_ptr<CodeBlock>>(c, dfg->createCodeBlock())));
}
static std::vector<std::vector<std::vector<std::vector<std::shared_ptr<CodeBlock>>>>> make_multiDim_CodeBlocks(std::shared_ptr<DataFlowGraph> dfg, int a, int b, int c, int d) {
    return std::vector<std::vector<std::vector<std::vector<std::shared_ptr<CodeBlock>>>>>(a, std::vector<std::vector<std::vector<std::shared_ptr<CodeBlock>>>>(b, std::vector<std::vector<std::shared_ptr<CodeBlock>>>(c, std::vector<std::shared_ptr<CodeBlock>>(d, dfg->createCodeBlock()))));
}
static std::vector<std::vector<std::vector<std::vector<std::vector<std::shared_ptr<CodeBlock>>>>>> make_multiDim_CodeBlocks(std::shared_ptr<DataFlowGraph> dfg, int a, int b, int c, int d, int e) {
    return std::vector<std::vector<std::vector<std::vector<std::vector<std::shared_ptr<CodeBlock>>>>>>(a, std::vector<std::vector<std::vector<std::vector<std::shared_ptr<CodeBlock>>>>>(b, std::vector<std::vector<std::vector<std::shared_ptr<CodeBlock>>>>(c, std::vector<std::vector<std::shared_ptr<CodeBlock>>>(d, std::vector<std::shared_ptr<CodeBlock>>(e, dfg->createCodeBlock())))));
}

std::shared_ptr<DataFlowGraph> gemm_tc_m2048_k4096_n4096(PEArray & pe_array) {
    using namespace std;
    std::shared_ptr<DataFlowGraph> dfg = std::make_shared<DataFlowGraph>();

    /* load A [128, 256], number of (16, 16) blocks is [8, 16] */
    auto load_A = make_multiDim_CodeBlocks(dfg, PE_ROWS, NUM_K_OUTER, NUM_M_TILE, NUM_K_TILE);
    auto copy_A = make_multiDim_CodeBlocks(dfg, PE_ROWS*(PE_COLS-1), NUM_K_OUTER, NUM_M_TILE, NUM_K_TILE);
    auto locate_copy_A = [](int row, int col) {return row*(PE_COLS-1)+col-1;};
    //vector<vector<vector<shared_ptr<CodeBlock>>>> load_A(NUM_K_OUTER, vector<vector<shared_ptr<CodeBlock>>>(NUM_M_TILE, vector<shared_ptr<CodeBlock>>(NUM_K_TILE, dfg->createCodeBlock())));
    /* load B [256, 256], number of (16, 16) blocks is [16, 16] */
    auto load_B = make_multiDim_CodeBlocks(dfg, PE_COLS, NUM_K_OUTER, NUM_K_TILE, NUM_N_TILE);
    auto copy_B = make_multiDim_CodeBlocks(dfg, (PE_ROWS-1)*PE_COLS, NUM_K_OUTER, NUM_K_TILE, NUM_N_TILE);
    auto locate_copy_B = [](int row, int col){return (row-1)*PE_COLS+col;};
    // vector<vector<vector<shared_ptr<CodeBlock>>>> load_B(NUM_K_OUTER, vector<vector<shared_ptr<CodeBlock>>>(NUM_K_TILE, vector<shared_ptr<CodeBlock>>(NUM_N_TILE, dfg->createCodeBlock())));
    /* Calculate m=k=n=16 matrix block using tensor core in each CodeBlock
     * all in all: (128, 256) * (256, 256) = (128, 256) */
    auto compute = make_multiDim_CodeBlocks(dfg, PE_ROWS*PE_COLS, NUM_K_OUTER, NUM_M_TILE, NUM_N_TILE, NUM_K_TILE);
    // vector<vector<vector<vector<shared_ptr<CodeBlock>>>>> compute(NUM_K_OUTER, vector<vector<vector<shared_ptr<CodeBlock>>>>(NUM_M_TILE, vector<vector<shared_ptr<CodeBlock>>>(NUM_N_TILE, vector<shared_ptr<CodeBlock>>(NUM_K_TILE, dfg->createCodeBlock())));
    /* store result [128, 256], number of (16, 16) blocks is [8, 16] */
    auto store_result = make_multiDim_CodeBlocks(dfg, PE_ROWS*PE_COLS, NUM_M_TILE, NUM_N_TILE);

    for(int row = 0; row < PE_ROWS; row++)
        for(int col = 0; col < PE_COLS; col++)
            for(int k_outer = 0; k_outer < NUM_K_OUTER; k_outer++)
                for(int i = 0; i < NUM_M_TILE; i++)
                    for(int j = 0; j < NUM_N_TILE; j++)
                        for(int k = 0; k < NUM_K_TILE; k++) {
                            if(col==0)
                                dfg->connectCodeBlock(load_A[row][k_outer][i][k], compute[row*PE_COLS+col][k_outer][i][j][k]);
                            else
                                dfg->connectCodeBlock(copy_A[locate_copy_A(row, col)][k_outer][i][k], compute[row*PE_COLS+col][k_outer][i][j][k]);
                            if(row==0)
                                dfg->connectCodeBlock(load_B[col][k_outer][k][j], compute[row*PE_COLS+col][k_outer][i][j][k]);
                            else
                                dfg->connectCodeBlock(copy_B[locate_copy_B(row, col)][k_outer][k][j], compute[row*PE_COLS+col][k_outer][i][j][k]);
                        }

    for(int row = 0; row < PE_ROWS; row++)
        for(int col = 0; col < PE_COLS; col++)
            for(int k_outer=1; k_outer<NUM_K_OUTER; k_outer++)
                for(int i=0; i<NUM_M_TILE; i++)
                    for(int j=0; j<NUM_N_TILE; j++)
                        for(int k=0; k<NUM_K_TILE; k++){
                            if(col==0)
                                dfg->connectCodeBlock(compute[row*PE_COLS+col][k_outer-1][i][j][k], load_A[row][k_outer][i][k]);
                            else
                                dfg->connectCodeBlock(compute[row*PE_COLS+col][k_outer-1][i][j][k], copy_A[locate_copy_A(row, col)][k_outer][i][k]);
                            if(row==0)
                                dfg->connectCodeBlock(compute[row*PE_COLS+col][k_outer-1][i][j][k], load_B[col][k_outer][k][j]);
                            else
                                dfg->connectCodeBlock(compute[row*PE_COLS+col][k_outer-1][i][j][k], copy_B[locate_copy_B(row, col)][k_outer][k][j]);
                        }
    for(int row = 0; row < PE_ROWS; row++)
        for(int col = 0; col < PE_COLS; col++){
            int k_outer = NUM_K_OUTER-1;
            for(int i=0; i<NUM_M_TILE; i++)
                for(int j=0; j<NUM_N_TILE; j++)
                    for(int k=0; k<NUM_K_TILE; k++){
                        dfg->connectCodeBlock(compute[row*PE_COLS+col][k_outer][i][j][k], store_result[row*PE_COLS+col][i][j]);
                    }
        }

    return dfg;
}