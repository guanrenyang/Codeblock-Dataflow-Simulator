#ifndef CODEBLOCK_H
#define CODEBLOCK_H

#include <set>

#include "common.h"
#include "Inst.h"

class CodeBlock {
    int constraint_cnt;
    std::vector<std::shared_ptr<Inst>> inst_stream;
    std::set<std::shared_ptr<CodeBlock>> to_signal; // downstream CodeBlocks

public:
    CodeBlock(): constraint_cnt(0) {}
    void append_instruction(const std::shared_ptr<Inst>& inst_ptr) {
        inst_stream.push_back(inst_ptr);
    }
    void add_constraint() {
        constraint_cnt++;
    }
    void add_downstream(const std::shared_ptr<CodeBlock>& cb) {
        to_signal.insert(cb);
        cb->add_constraint();
    }

    bool empty() {
        return inst_stream.empty();
    }
    std::shared_ptr<Inst> popInstruction() {
        auto inst = inst_stream.back();
        inst_stream.pop_back();
        return inst;
    }
};
#endif