#include "LocalScheduler.h"
#include "CodeBlock.h"
#include "Inst.h"

void LocalScheduler:: addCodeBlock(std::shared_ptr<CodeBlock> code_block) {
    /* This is now a naive implementation for testing
        TODO: Place the four types of instructions in a CodeBlock into their respective instruction queues.
    */
    waiting_CodeBlocks.insert(code_block);
};

template<typename InstType>
std::shared_ptr<Inst> LocalScheduler::getReadyInstruction() {
    check_waiting_CodeBlocks();

    if (ready_CodeBlocks.empty()) {
        return std::make_shared<NopInst>();
    }

    std::shared_ptr<Inst> return_inst = std::make_shared<NopInst>();
    for (auto & ready_CodeBlock : ready_CodeBlocks) {
        if (ready_CodeBlock->has_valid_instruction()) {
            auto front_inst = ready_CodeBlock->frontInstruction();
            if (std::dynamic_pointer_cast<InstType>(front_inst) != nullptr) { // select instruction type
                return_inst = ready_CodeBlock->popInstruction();
                break;
            }
            /* TODO: maybe problem -- CodeBlocks will be left in `ready_CodeBlocks` list when finished */
        }
    }

    return return_inst;
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

// explicit instantiation
template std::shared_ptr<Inst> LocalScheduler::getReadyInstruction<LdInst>();
template std::shared_ptr<Inst> LocalScheduler::getReadyInstruction<StInst>();
template std::shared_ptr<Inst> LocalScheduler::getReadyInstruction<CalInst>();
template std::shared_ptr<Inst> LocalScheduler::getReadyInstruction<CopyInst>();