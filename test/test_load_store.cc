#include "../include/Inst.h"
#include "../include/PEArray.h"
#include <memory>

int main() {
    std::list<std::shared_ptr<Inst>> inst_list;
    std::list<std::shared_ptr<CodeBlock>> code_block_list;

// TODO: add a CodeBlockBuilder to manage all the CodeBlocks, instructions and dependencies among them
  //cal inst: opcode, dst, src0, src1
  std::shared_ptr<Inst> test_inst_add = std::make_shared<CalInst>(0, 0, 0, 1);
    inst_list.push_back(test_inst_add);

  //load: row, col, idx, addr
  //copy: src_row, src_col, dst_row, dst_col, src_reg, dst_reg
  // std::shared_ptr<Inst> test_copy = std::make_shared<CopyInst>(0, 0, 2, 0, 1, 0);
    // inst_list.push_back(test_copy);
    // test_inst_add->connect_to(test_copy);

  std::shared_ptr<Inst> test_inst_store = std::make_shared<StInst>(0, 0, 0, 0);
    inst_list.push_back(test_inst_store);
    test_inst_add->connect_to(test_inst_store);

  std::shared_ptr<Inst> test_inst_store_2 = std::make_shared<StInst>(0, 0, 0, 128);
    inst_list.push_back(test_inst_store_2);
    test_inst_store->connect_to(test_inst_store_2);

  std::shared_ptr<Inst> test_inst_load = std::make_shared<LdInst>(0, 1, 0, 128);
    inst_list.push_back(test_inst_load);

  std::shared_ptr<Inst> test_inst_load_2 = std::make_shared<LdInst>(0, 1, 1, 0);
    inst_list.push_back(test_inst_load_2);
    test_inst_load->connect_to(test_inst_load_2);

  std::shared_ptr<CodeBlock> code_block_0 = std::make_shared<CodeBlock>();
  code_block_list.push_back(code_block_0);
  std::shared_ptr<CodeBlock> code_block_1 = std::make_shared<CodeBlock>();
  code_block_list.push_back(code_block_1);

  code_block_0->append_instruction(test_inst_add);
  //code_block_0->append_instruction(test_inst_load);
  //code_block_0->append_instruction(test_copy);
  code_block_0->append_instruction(test_inst_store);
  //code_block_0->append_instruction(test_inst_add);
  code_block_0->append_instruction(test_inst_store_2);

  code_block_1->append_instruction(test_inst_load);
  code_block_1->append_instruction(test_inst_load_2);

  code_block_0->connect_to(code_block_1);

  PEArray pe_array(4, 4, 6*1024*1024);
  pe_array.add_CodeBlock(0, 0, code_block_0);
  pe_array.add_CodeBlock(0, 1, code_block_1);

      std::cout << " Original :\n";
      pe_array.display_reg(0, 0, 0);
      pe_array.display_reg(0, 0, 1);
      pe_array.display_reg(0, 1, 0);
      pe_array.display_reg(0, 1, 1);
      pe_array.display_spm(0);
      pe_array.display_spm(128);
      std::cout << std::endl;

  int cycles = 50;
  for (int i = 1; i <= cycles; i++) {
      // PE performs execution at the cycle,
#ifdef DEBUG
      std::cout << "----------Start cycle " << i << "----------" << std::endl;
#endif
      pe_array.execute_cycle();

      // show result
      std::cout << "After cycle " << i << " :\n";
      pe_array.display_reg(0, 0, 0);
      pe_array.display_reg(0, 0, 1);
      pe_array.display_reg(0, 1, 0);
      pe_array.display_reg(0, 1, 1);
      pe_array.display_spm(0);
      pe_array.display_spm(128);
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
  // pe.display_reg_as_fp32(0);
  // pe.display_reg_as_fp32(1);
  // pe.display_reg_as_fp32(2);
  // pe.display_reg_as_fp32(3);
  // pe.display_reg_as_fp32(4);
  return 0;
}
