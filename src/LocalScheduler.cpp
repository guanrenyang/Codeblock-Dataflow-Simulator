#include "LocalScheduler.h"
#include "CodeBlock.h"
#include "Inst.h"

void LocalScheduler:: addCodeBlock(std::shared_ptr<CodeBlock> code_block) {
    /* This is now a naive implementation for testing
        TODO: Place the four types of instructions in a CodeBlock into their respective instruction queues.
    */
    waiting_CodeBlocks.insert(code_block);
};

std::shared_ptr<Inst> LocalScheduler::getReadyInstruction() {
    check_waiting_CodeBlocks();

    if (ready_CodeBlocks.empty()) {
        return std::make_shared<NopInst>();
    }

    std::shared_ptr<Inst> return_inst = std::make_shared<NopInst>();

    for (auto iter = ready_CodeBlocks.begin(); iter != ready_CodeBlocks.end(); iter++) {
        if ((*iter)->has_valid_instruction()) {
            return_inst = (*iter)->popInstruction();
            /* TODO: maybe problem -- CodeBlocks will be left in `ready_CodeBlocks` list when finished */
        }
    }

    return return_inst;

/*    if (current_CodeBlock == nullptr || current_CodeBlock->is_finished()) { // get next ready CodeBlock
        if(ready_CodeBlocks.empty()) {
            return std::make_shared<NopInst>();
        }
        current_CodeBlock = ready_CodeBlocks.front();
        ready_CodeBlocks.pop_front();
    }

    *//* we can't manage validness of instruction in CodeBlock
     * Validness of instruction and codeblock are managed here
     * *//*
    auto inst = current_CodeBlock->popInstruction(); // The instruction must be valid becasuse of the check above

    return inst;*/
}

void LocalScheduler::check_waiting_CodeBlocks() {
    // move the CodeBlock with constraint_cnt==0 from waiting to ready
    std::set<std::shared_ptr<CodeBlock>> just_ready;
    for (const auto& cb : waiting_CodeBlocks) {
        if (cb->ready()) {
            just_ready.insert(cb);
        }
    }
    for (const auto& cb: just_ready) {
        ready_CodeBlocks.push_back(cb);
        waiting_CodeBlocks.erase(cb);
    }
}