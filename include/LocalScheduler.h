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
        ready_CodeBlocks.emplace_back(CodeBlock);
    };
    std::shared_ptr<Inst> getReadyInstruction() {
        if(current_CodeBlock == nullptr || current_CodeBlock->empty()) {
            if(ready_CodeBlocks.empty()) {
                return std::make_shared<NopInst>();
            }
            current_CodeBlock = ready_CodeBlocks.back();
            ready_CodeBlocks.pop_back();
        }

        auto inst = current_CodeBlock->popInstruction();

        if(current_CodeBlock->empty()) {
            current_CodeBlock = nullptr;
            return std::make_shared<NopInst>();
        }
        return inst;
    };
private:
    std::vector<std::shared_ptr<CodeBlock>> waiting_CodeBlocks;
    std::vector<std::shared_ptr<CodeBlock>> ready_CodeBlocks;
    std::shared_ptr<CodeBlock> current_CodeBlock;
};

#endif //SCHEDULER_H
