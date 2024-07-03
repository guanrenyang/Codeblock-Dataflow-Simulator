#include "../include/Inst.h"
#include "../include/PEArray.h"
#include <memory>

int main() {
  //cal inst: opcode, dst, src0, src1
  std::shared_ptr<Inst> test_inst_add = std::make_shared<CalInst>(0, 0, 0, 1);
  //load: row, col, idx, addr
  //copy: src_row, src_col, dst_row, dst_col, src_reg, dst_reg
  std::shared_ptr<Inst> test_copy = std::make_shared<CopyInst>(0, 0, 2, 0, 1, 0);

  std::shared_ptr<Inst> test_inst_store = std::make_shared<StInst>(0, 0, 0, 0);
  std::shared_ptr<Inst> test_inst_store_2 = std::make_shared<StInst>(0, 0, 0, 128);

  std::shared_ptr<Inst> test_inst_load = std::make_shared<LdInst>(0, 1, 0, 128);
  std::shared_ptr<Inst> test_inst_load_2 = std::make_shared<LdInst>(0, 1, 1, 0);


  std::shared_ptr<CodeBlock> code_block_0 = std::make_shared<CodeBlock>();
  std::shared_ptr<CodeBlock> code_block_1 = std::make_shared<CodeBlock>();

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

  int cycles = 30;
  for (int i = 1; i <= cycles; i++) {
      // PE performs execution at the cycle,
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
      code_block_0->signal_downstream();
      // code_block_1->signal_downstream();

      code_block_0->update_constraint();
      code_block_1->update_constraint();

      std::cout<< "Code Block 0 state: " << code_block_0->empty() << std::endl;
  }
  // pe.display_reg_as_fp32(0);
  // pe.display_reg_as_fp32(1);
  // pe.display_reg_as_fp32(2);
  // pe.display_reg_as_fp32(3);
  // pe.display_reg_as_fp32(4);
  return 0;
}
