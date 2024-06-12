#include <algorithm>

#include "include/common.h"
#include "include/PE.h"
#include "include/SPM.h"

int main() {
  // Front-end: Define a dataflow graph
  std::shared_ptr<Inst> test_inst = std::make_shared<CalInst>(0, 0, 1, 2);

  std::shared_ptr<CodeBlock> code_block = std::make_shared<CodeBlock>();
  code_block->append_instruction(test_inst);

  // A local scheduler is affined to a PE, and it holds a sub-DFG
  std::unique_ptr<LocalScheduler> scheduler = std::make_unique<LocalScheduler>();
  scheduler->addCodeBlock(code_block);

  // Back-end: PE excuting the dataflow graph 
  std::shared_ptr<SPM> spm = std::make_shared<SPM>();
  PE pe(std::move(scheduler), spm);

  int cycles = 100;
  for (int i = 0; i < cycles; i++) {
    pe.execute_cycle();
  }

  pe.display_regfile();
  return 0;
}
