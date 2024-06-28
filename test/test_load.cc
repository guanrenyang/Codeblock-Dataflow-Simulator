#include "../include/Inst.h"
#include "../include/PEArray.h"
#include <memory>

int main() {
  // Front-end: Define a dataflow graph
  std::shared_ptr<Inst> test_load_0 = std::make_shared<LdInst>(1, 1, 2, 0);
  std::shared_ptr<CodeBlock> code_block_0 = std::make_shared<CodeBlock>();
  code_block_0->append_instruction(test_load_0);

  std::shared_ptr<Inst> test_load_1 = std::make_shared<LdInst>(0, 0, 0, 128);
  std::shared_ptr<CodeBlock> code_block_1 = std::make_shared<CodeBlock>();
  code_block_0->append_instruction(test_load_1);

  code_block_0->connect_to(code_block_1);

  // Back-end: PE excuting the dataflow graph
  PEArray pe_array(4, 4, 6*1024*1024);
  pe_array.add_CodeBlock(0, 0, code_block_0);
  pe_array.add_CodeBlock(0, 1, code_block_1);

  int cycles = 4;
  for (int i = 0; i < cycles; i++) {
      // PE performs execution at the cycle,
      pe_array.execute_cycle();

      // show result
      std::cout << "After cycle " << i << " :\n";
      pe_array.display_reg(1, 1, 2);
      pe_array.display_reg(0, 0, 0);
      std::cout << std::endl;

      // TODO: add a CodeBlock Manager to manage all the CodeBlocks scattered on the chip
      // update the constraints of each code block
      code_block_0->signal_downstream();
      code_block_1->signal_downstream();

      // code_block_0->update_constraint();
      code_block_1->update_constraint();

  }
  // pe.display_reg_as_fp32(0);
  // pe.display_reg_as_fp32(1);
  // pe.display_reg_as_fp32(2);
  // pe.display_reg_as_fp32(3);
  // pe.display_reg_as_fp32(4);
  return 0;
}
