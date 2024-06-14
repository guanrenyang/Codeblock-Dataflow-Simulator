#ifndef CODEBLOCK_H
#define CODEBLOCK_H

#include <memory>
#include <set>

#include "common.h"
#include "Inst.h"

class CodeBlock {

public:
    CodeBlock(): constraint_cnt(0) {}

    /* TODO:
        When a CodeBlock has executed all its instructions, it signals all is downstream CodeBlocks
        A CodeBlock is ready to execute when all its upstream CodeBlocks have finished executing
    */
    // add a count of upstream CodeBlock to the current CodeBlock


    // add a downstream CodeBlock to the current CodeBlock
    void connect_to(const std::shared_ptr<CodeBlock>& cb) {
        to_signal.insert(cb);
        cb->add_constraint();
    }

    void append_instruction(const std::shared_ptr<Inst>& inst_ptr) {
        inst_stream.push_back(inst_ptr);
    }

    bool empty() {
        return inst_stream.empty();
    }

    bool ready() {
        return constraint_cnt==0;
    }

    std::shared_ptr<Inst> popInstruction() {
        if (inst_stream.empty())
            return std::make_shared<NopInst>();

        auto inst = inst_stream.back();
        inst_stream.pop_back();

        if (empty()) {
            // signal all downstream CodeBlocks
            for (const auto& cb : to_signal) {
                cb->constraint_cnt--;
            }
        }

        return inst;
    }

private:
    int constraint_cnt;
    std::vector<std::shared_ptr<Inst>> inst_stream;
    std::set<std::shared_ptr<CodeBlock>> to_signal; // downstream CodeBlocks

    void add_constraint() {
        constraint_cnt++;
    }
};

#endif