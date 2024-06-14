#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <memory>
#include <memory>
#include <queue>
#include <vector>

#include "CodeBlock.h"

/* In current design, Scheduler is assgined to each PE */
class LocalScheduler {
public:
    void addCodeBlock(const std::shared_ptr<CodeBlock>& CodeBlock) {
        /* This is now a naive implementation for testing
            TODO: Place the four types of instructions in a CodeBlock into their respective instruction queues.
        */
        waiting_CodeBlocks.insert(CodeBlock);
    };

    std::shared_ptr<Inst> getReadyInstruction() {
        check_waiting();

        if(current_CodeBlock == nullptr || current_CodeBlock->empty()) { // get next ready CodeBlock
            if(ready_CodeBlocks.empty()) {
                return std::make_shared<NopInst>();
            }
            current_CodeBlock = ready_CodeBlocks.front();
            ready_CodeBlocks.pop();
        }

        auto inst = current_CodeBlock->popInstruction(); // The instruction must be valid becasuse of the check above

        return inst;
    };
private:
    std::shared_ptr<CodeBlock> current_CodeBlock;

    std::set<std::shared_ptr<CodeBlock>> waiting_CodeBlocks;
    std::queue<std::shared_ptr<CodeBlock>> ready_CodeBlocks;
    // The two queues above are exclusive

    void check_waiting() {
        // move the CodeBlock with constraint_cnt==0 from waiting to ready
        std::set<std::shared_ptr<CodeBlock>> just_ready;
        for (const auto& cb : waiting_CodeBlocks) {
            if (cb->ready()) {
                just_ready.insert(cb);
            }
        }
        for (const auto& cb: just_ready) {
            ready_CodeBlocks.push(cb);
            waiting_CodeBlocks.erase(cb);
        }
    }
};

#endif //SCHEDULER_H
