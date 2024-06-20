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

    if(current_CodeBlock == nullptr || current_CodeBlock->empty()) { // get next ready CodeBlock
        if(ready_CodeBlocks.empty()) {
            return std::make_shared<NopInst>();
        }
        current_CodeBlock = ready_CodeBlocks.front();
        ready_CodeBlocks.pop();
    }

    auto inst = current_CodeBlock->popInstruction(); // The instruction must be valid becasuse of the check above

    // TODO: call the wait method of the instruction to add it to the waiting set
            
    // TODO: if inst_stream is empty then return a NopInst

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