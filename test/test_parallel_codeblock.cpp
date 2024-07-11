#include "../include/Inst.h"
#include "../include/PEArray.h"
#include "../include/DataFlowGraph.h"

static int use_copy(){
    DataFlowGraph dfg;

    std::shared_ptr<CodeBlock> code_block_0 = dfg.createCodeBlock();
    std::shared_ptr<CodeBlock> code_block_1 = dfg.createCodeBlock();
    std::shared_ptr<CodeBlock> code_block_2 = dfg.createCodeBlock();

    // code block 0
    dfg.appendCal(code_block_0, 0, 0, 0, 1);
    dfg.appendCopy(code_block_0, 0, 0, 0, 0, 1, 0);

    // code block 1
    dfg.appendCal(code_block_1, 0, 2, 2, 3);
    dfg.appendCopy(code_block_1, 0, 0, 2, 0, 1, 1);

    // codeblock 2
    dfg.appendCal(code_block_2, 0, 0, 0, 1);
    dfg.appendStore(code_block_2, 0, 1, 0, 256);

    dfg.connectCodeBlock(code_block_0, code_block_2);
    dfg.connectCodeBlock(code_block_1, code_block_2);

    PEArray pe_array(4, 4, 6*1024*1024);
    pe_array.add_CodeBlock(0, 0, code_block_0);
    pe_array.add_CodeBlock(0, 0, code_block_1);
    pe_array.add_CodeBlock(0, 1, code_block_2);

    std::cout << " Original :\n";
    pe_array.display_reg(0, 0, 0);
    pe_array.display_reg(0, 0, 1);
    pe_array.display_reg(0, 0, 2);
    pe_array.display_reg(0, 1, 0);
    pe_array.display_reg(0, 1, 1);
    pe_array.display_spm(0);
    pe_array.display_spm(128);
    pe_array.display_spm(256);
    std::cout << std::endl;

    int cycles = 50;
    for (int i = 1; i <= cycles; i++) {
        // PE performs execution at the cycle,
        std::cout << "----------Start cycle " << i << "----------" << std::endl;

        pe_array.execute_cycle();

        // show result
        std::cout << "After cycle " << i << " :\n";
        pe_array.display_reg(0, 0, 0);
        pe_array.display_reg(0, 0, 1);
        pe_array.display_reg(0, 0, 2);
        pe_array.display_reg(0, 1, 0);
        pe_array.display_reg(0, 1, 1);
        pe_array.display_spm(0);
        pe_array.display_spm(128);
        pe_array.display_spm(256);
        std::cout << std::endl;

        dfg.signal_and_update();
    }
    return 0;
}
static int use_ldst(){
    DataFlowGraph dfg;

    std::shared_ptr<CodeBlock> code_block_0 = dfg.createCodeBlock();
    std::shared_ptr<CodeBlock> code_block_1 = dfg.createCodeBlock();
    std::shared_ptr<CodeBlock> code_block_2 = dfg.createCodeBlock();

    // code block 0
    dfg.appendCal(code_block_0, 0, 0, 0, 1);
    dfg.appendStore(code_block_0, 0, 0, 0, 0);

    // code block 1
    dfg.appendCal(code_block_1, 0, 2, 2, 3);
    dfg.appendStore(code_block_1, 0, 0, 2, 128);

    // codeblock 2
    dfg.appendLoad(code_block_2, 0, 1, 0, 0);
    dfg.appendLoad(code_block_2, 0, 1, 1, 128);
    dfg.appendCal(code_block_2, 0, 0, 0, 1);
    dfg.appendStore(code_block_2, 0, 1, 0, 256);

    dfg.connectCodeBlock(code_block_0, code_block_2);
    dfg.connectCodeBlock(code_block_1, code_block_2);

    PEArray pe_array(4, 4, 6*1024*1024);
    pe_array.add_CodeBlock(0, 0, code_block_0);
    pe_array.add_CodeBlock(0, 0, code_block_1);
    pe_array.add_CodeBlock(0, 1, code_block_2);

    std::cout << " Original :\n";
    pe_array.display_reg(0, 0, 0);
    pe_array.display_reg(0, 0, 1);
    pe_array.display_reg(0, 0, 2);
    pe_array.display_reg(0, 1, 0);
    pe_array.display_reg(0, 1, 1);
    pe_array.display_spm(0);
    pe_array.display_spm(128);
    pe_array.display_spm(256);
    std::cout << std::endl;

    int cycles = 50;
    for (int i = 1; i <= cycles; i++) {
        // PE performs execution at the cycle,
        std::cout << "----------Start cycle " << i << "----------" << std::endl;

        pe_array.execute_cycle();

        // show result
        std::cout << "After cycle " << i << " :\n";
        pe_array.display_reg(0, 0, 0);
        pe_array.display_reg(0, 0, 1);
        pe_array.display_reg(0, 0, 2);
        pe_array.display_reg(0, 1, 0);
        pe_array.display_reg(0, 1, 1);
        pe_array.display_spm(0);
        pe_array.display_spm(128);
        pe_array.display_spm(256);
        std::cout << std::endl;

        // TODO: add a CodeBlock Manager to manage all the CodeBlocks scattered on the chip
        // update the constraints of each code block
        // code_block_0->signal_downstream_if_finished();
        // code_block_1->signal_downstream_if_finished();

        dfg.signal_and_update();
    }
    return 0;
}
int main() {
    return use_copy();
}
