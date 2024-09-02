#include "PEArray.h"
#include "DataFlowGraph.h"

int display(PEArray & pe_array, int cycle) {
    if (cycle != 0)
        std::cout << "After cycle " << cycle << " :\n";
    else
        std::cout << " Original :\n";

    std::cout << "RAW_DATA Registers:\n";
    for (int reg_idx = 0; reg_idx < 8; reg_idx++)
        pe_array.display_reg_as<half>(0, 0, reg_idx);

    std::cout << "Result Registers:\n";
    for (int reg_idx = 8; reg_idx < 12; reg_idx++)
        pe_array.display_reg_as<half>(0, 0, reg_idx);

    std::cout << "SPM Input:\n";
    for (int addr = 0; addr < 8 * 128; addr+=128)
        pe_array.display_spm_in_vector_as<half>(addr);

    std::cout << "SPM Output:\n";
    for (int addr = 8 * 128; addr < 12 * 128; addr += 128)
        pe_array.display_spm_in_vector_as<half>(addr);

    std::cout << std::endl;
    return 0;
}
std::shared_ptr<DataFlowGraph> matmul_16_16_16(PEArray & pe_array){
    std::shared_ptr<DataFlowGraph> dfg = std::make_shared<DataFlowGraph>();

    std::shared_ptr<CodeBlock> load_cb = dfg->createCodeBlock();
    std::shared_ptr<CodeBlock> cp_cb = dfg->createDependentCodeBlock(load_cb);
    std::shared_ptr<CodeBlock> cal_cb = dfg->createDependentCodeBlock(cp_cb);
    std::shared_ptr<CodeBlock> store_cb = dfg->createDependentCodeBlock(cal_cb);

    pe_array.add_CodeBlock(0, 3, load_cb);
    pe_array.add_CodeBlock(0,3, cp_cb);
    pe_array.add_CodeBlock(0, 0, cal_cb);
    pe_array.add_CodeBlock(0, 0, store_cb);

    // prepare input
    dfg->appendLoad(load_cb, 0, 3, 0, 0*128, true);
    dfg->appendLoad(load_cb, 0, 3, 4, 4*128, true);

    dfg->appendCopy(cp_cb, 0, 3, 0, 0, 0, 0, true);
    dfg->appendCopy(cp_cb, 0, 3, 4, 0, 0, 4, true);

    // calculation
    dfg->appendTensorCal(cal_cb, 0, 8, 0, 4);
    // dfg->appendCal(cal_cb, 0, 100, 100, 100);

    // store result
    dfg->appendStore(store_cb, 0, 0, 8, 8*128, true);

    return dfg;
}

int main() {
    PEArray pe_array(4, 4, 6*1024*1024);

    auto dfg = matmul_16_16_16(pe_array);

    display(pe_array, 0);
    int cycle_idx = 0;
    while(!dfg->is_finished()) {
        // PE performs execution at the cycle,
        std::cout << "----------Start cycle " << cycle_idx << "----------" << std::endl;

        pe_array.execute_cycle();

        display(pe_array, cycle_idx);

        dfg->signal_and_update();

        cycle_idx++;
    }

    std::cout<<"Simulation finished after " <<cycle_idx << " cycles" <<std::endl;

    return 0;
}
