#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <memory>
#include <memory>
#include <vector>

#include "CodeBlock.h"

/* In current design, Scheduler is assgined to each PE */
class LocalScheduler {
public:
    void addCodeBlock(const std::shared_ptr<CodeBlock>& CodeBlock) {
        /* This is now a naive implementation for testing
            TODO: Place the four types of instructions in a CodeBlock into their respective instruction queues.
        */
        ready_CodeBlocks.emplace_back(CodeBlock);
    };

    std::shared_ptr<Inst> getReadyInstruction() {
        if(current_CodeBlock == nullptr || current_CodeBlock->empty()) { // get next ready CodeBlock
            if(ready_CodeBlocks.empty()) {
                return std::make_shared<NopInst>();
            }
            current_CodeBlock = ready_CodeBlocks.back();
            ready_CodeBlocks.pop_back();
        }

        auto inst = current_CodeBlock->popInstruction(); // The instruction must be valid becasuse of the check above

        return inst;
    };
private:
    std::shared_ptr<CodeBlock> current_CodeBlock;

    std::vector<std::shared_ptr<CodeBlock>> waiting_CodeBlocks;
    std::vector<std::shared_ptr<CodeBlock>> ready_CodeBlocks;
    // The two queues above are exclusive
};

#endif //SCHEDULER_H
