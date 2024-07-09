#include "../include/Inst.h"
#include "../include/PEArray.h"
#include <memory>
static int use_copy(){
    std::list<std::shared_ptr<Inst>> inst_list;
    std::list<std::shared_ptr<CodeBlock>> code_block_list;

    std::shared_ptr<CodeBlock> code_block_0 = std::make_shared<CodeBlock>();
    code_block_list.push_back(code_block_0);
    std::shared_ptr<CodeBlock> code_block_1 = std::make_shared<CodeBlock>();
    code_block_list.push_back(code_block_1);
    std::shared_ptr<CodeBlock> code_block_2 = std::make_shared<CodeBlock>();
    code_block_list.push_back(code_block_2);

    // code block 0
    std::shared_ptr<Inst> add_0 = std::make_shared<CalInst>(0, 0, 0, 1);
    std::shared_ptr<Inst> copy_0 = std::make_shared<CopyInst>(0, 0, 0, 0, 1, 0);
    code_block_0->append_instruction(add_0);
    code_block_0->append_instruction(copy_0);
    inst_list.push_back(add_0);
    inst_list.push_back(copy_0);
    add_0->connect_to(copy_0);

    // code block 1
    std::shared_ptr<Inst> add_1 = std::make_shared<CalInst>(0, 2, 2, 3);
    std::shared_ptr<Inst> copy_1 = std::make_shared<CopyInst>(0, 0, 2, 0, 1, 1);
    code_block_1->append_instruction(add_1);
    code_block_1->append_instruction(copy_1);
    inst_list.push_back(add_1);
    inst_list.push_back(copy_1);
    add_1->connect_to(copy_1);

    // codeblock 2
    std::shared_ptr<Inst> add_2 = std::make_shared<CalInst>(0, 0, 0, 1);
    std::shared_ptr<Inst> store_2 = std::make_shared<StInst>(0, 1, 0,  256);
    code_block_2->append_instruction(add_2);
    code_block_2->append_instruction(store_2);
    inst_list.push_back(add_2);
    inst_list.push_back(store_2);
    add_2->connect_to(store_2);


    code_block_0->connect_to(code_block_2);
    code_block_1->connect_to(code_block_2);


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

        for (const auto& code_block: code_block_list) {
            code_block->signal_downstream_if_finished();
        }
        for (const auto& code_block: code_block_list) {
            code_block->update_constraint();
        }
        for (const auto& inst : inst_list) {
            inst->signal_downstream_if_finished();
        }
        for (const auto& inst : inst_list) {
            inst->update_constraint();
        }
    }
    return 0;
}
static int use_ldst(){
    std::list<std::shared_ptr<Inst>> inst_list;
    std::list<std::shared_ptr<CodeBlock>> code_block_list;

    std::shared_ptr<CodeBlock> code_block_0 = std::make_shared<CodeBlock>();
    code_block_list.push_back(code_block_0);
    std::shared_ptr<CodeBlock> code_block_1 = std::make_shared<CodeBlock>();
    code_block_list.push_back(code_block_1);
    std::shared_ptr<CodeBlock> code_block_2 = std::make_shared<CodeBlock>();
    code_block_list.push_back(code_block_2);

    // code block 0
    std::shared_ptr<Inst> add_0 = std::make_shared<CalInst>(0, 0, 0, 1);
    std::shared_ptr<Inst> store_0 = std::make_shared<StInst>(0, 0, 0, 0);
    code_block_0->append_instruction(add_0);
    code_block_0->append_instruction(store_0);
    inst_list.push_back(add_0);
    inst_list.push_back(store_0);
    add_0->connect_to(store_0);

    // code block 1
    std::shared_ptr<Inst> add_1 = std::make_shared<CalInst>(0, 2, 2, 3);
    std::shared_ptr<Inst> store_1 = std::make_shared<StInst>(0, 0, 2,  128);
    code_block_1->append_instruction(add_1);
    code_block_1->append_instruction(store_1);
    inst_list.push_back(add_1);
    inst_list.push_back(store_1);
    add_1->connect_to(store_1);

    // codeblock 2
    std::shared_ptr<Inst> ld_2_0 = std::make_shared<LdInst>(0, 1, 0,  0);
    std::shared_ptr<Inst> ld_2_1 = std::make_shared<LdInst>(0, 1, 1,  128);
    std::shared_ptr<Inst> add_2 = std::make_shared<CalInst>(0, 0, 0, 1);
    std::shared_ptr<Inst> store_2 = std::make_shared<StInst>(0, 1, 0,  256);
    code_block_2->append_instruction(ld_2_0);
    code_block_2->append_instruction(ld_2_1);
    code_block_2->append_instruction(add_2);
    code_block_2->append_instruction(store_2);
    inst_list.push_back(ld_2_0);
    inst_list.push_back(ld_2_1);
    inst_list.push_back(add_2);
    inst_list.push_back(store_2);
    ld_2_0->connect_to(ld_2_1);
    ld_2_1->connect_to(add_2);
    add_2->connect_to(store_2);


    code_block_0->connect_to(code_block_2);
    code_block_1->connect_to(code_block_2);


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

        for (const auto& code_block: code_block_list) {
            code_block->signal_downstream_if_finished();
        }
        for (const auto& code_block: code_block_list) {
            code_block->update_constraint();
        }
        for (const auto& inst : inst_list) {
            inst->signal_downstream_if_finished();
        }
        for (const auto& inst : inst_list) {
            inst->update_constraint();
        }
    }
    return 0;
}
int main() {
    return use_ldst();
}
