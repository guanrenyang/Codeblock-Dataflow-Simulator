#include "LocalScheduler.h"
#include "CodeBlock.h"
#include "Inst.h"

// void AsyncInstManager::add_async_inst(std::shared_ptr<Inst> inst_ptr) {
//     waiting_inst.push_back(inst_ptr);
// };

// void AsyncInstManager::remove_async_inst(std::shared_ptr<Inst> inst_ptr) {
//     waiting_inst.remove(inst_ptr);
// };

// bool AsyncInstManager::empty() {
//     return waiting_inst.empty();
// };

void LocalScheduler:: addCodeBlock(std::shared_ptr<CodeBlock> code_block) {
    /* This is now a naive implementation for testing
        TODO: Place the four types of instructions in a CodeBlock into their respective instruction queues.
    */
    waiting_CodeBlocks.insert(code_block);
};

std::shared_ptr<Inst> LocalScheduler::getReadyInstruction() {
    check_waiting();

    /* TODO: Fix the following issue by eager signaling instead of lazy signaling
     * for now, if the last instruction of a CodeBlock on pe00 is an async block, the pe01 will be idle for an additional cycle
     * cycle 0: async instruction of pe00 is finished, at the same time the code block is finished
     * cycle 1: the pe00 senses that the current_CodeBlock is finished and signals the downstream, at the end of cycle1 the codeblock on pe01 is release
     * cycle 2: the pe01 get an instruction from its code block.
     */
    if (current_CodeBlock != nullptr && current_CodeBlock->is_finished()) {
        current_CodeBlock = nullptr;
    }
    if (current_CodeBlock == nullptr) { // get next ready CodeBlock
        if(ready_CodeBlocks.empty()) {
            return std::make_shared<NopInst>();
        }
        current_CodeBlock = ready_CodeBlocks.front();
        ready_CodeBlocks.pop();
    }

    auto inst = current_CodeBlock->popInstruction(); // The instruction must be valid becasuse of the check above

    return inst;
};

void LocalScheduler::check_waiting() {
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