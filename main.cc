#include <algorithm>

#include "include/common.h"
#include "include/PE.h"
#include "include/SPM.h"

#include "include/PEArray.h"

int main() {
  // Front-end: Define a dataflow graph
  std::shared_ptr<Inst> test_inst_0 = std::make_shared<CalInst>(0, 2, 0, 1);
  std::shared_ptr<CodeBlock> code_block_0 = std::make_shared<CodeBlock>();
  code_block_0->append_instruction(test_inst_0);

  std::shared_ptr<Inst> test_inst_1 = std::make_shared<CalInst>(0, 4, 2, 3);
  std::shared_ptr<CodeBlock> code_block_1 = std::make_shared<CodeBlock>();
  code_block_1->append_instruction(test_inst_1);

  code_block_0->connect_to(code_block_1);

  // Back-end: PE excuting the dataflow graph

  PEArray pe_array(4, 4, 6*1024*1024);
  pe_array.add_CodeBlock(0, 0, code_block_0);
  pe_array.add_CodeBlock(0, 1, code_block_1);

  int cycles = 2;
  for (int i = 0; i < cycles; i++) {
      // update the constraints of each code block
      code_block_0->update_constraint();
      code_block_1->update_constraint();

      // PE performs execution at the cycle,
      pe_array.execute_cycle();

      // show result
      std::cout << "After cycle " << i << " :\n";
      pe_array.display_reg(0, 0, 2);
      pe_array.display_reg(0, 1, 4);
      std::cout << std::endl;
  }
  // pe.display_reg_as_fp32(0);
  // pe.display_reg_as_fp32(1);
  // pe.display_reg_as_fp32(2);
  // pe.display_reg_as_fp32(3);
  // pe.display_reg_as_fp32(4);
  return 0;
}
