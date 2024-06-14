#include <algorithm>

#include "include/common.h"
#include "include/PE.h"
#include "include/SPM.h"

int main() {
  // Front-end: Define a dataflow graph
  std::shared_ptr<Inst> test_inst_0 = std::make_shared<CalInst>(0, 2, 0, 1);
  std::shared_ptr<CodeBlock> code_block_0 = std::make_shared<CodeBlock>();
  code_block_0->append_instruction(test_inst_0);

  std::shared_ptr<Inst> test_inst_1 = std::make_shared<CalInst>(0, 4, 2, 3);
  std::shared_ptr<CodeBlock> code_block_1 = std::make_shared<CodeBlock>();
  code_block_1->append_instruction(test_inst_1);

  code_block_0->connect_to(code_block_1);

  // A local scheduler is affined to a PE, and it holds a sub-DFG
  std::unique_ptr<LocalScheduler> scheduler = std::make_unique<LocalScheduler>();
  scheduler->addCodeBlock(code_block_0);
  scheduler->addCodeBlock(code_block_1);

  // Back-end: PE excuting the dataflow graph 
  std::shared_ptr<SPM> spm = std::make_shared<SPM>();
  PE pe(std::move(scheduler), spm);

  int cycles = 100;
  for (int i = 0; i < cycles; i++) {
    pe.execute_cycle();
  }

  pe.display_reg_as_fp32(0);
  pe.display_reg_as_fp32(1);
  pe.display_reg_as_fp32(2);
  pe.display_reg_as_fp32(3);
  pe.display_reg_as_fp32(4);
  return 0;
}
