#ifndef CODEBLOCK_H
#define CODEBLOCK_H

#include <set>

#include "common.h"
#include "Inst.h"

class CodeBlock {
    int constraint_cnt;
    std::vector<Inst> instructions;
    std::set<std::shared_ptr<CodeBlock>> to_signal; // downstream CodeBlocks

public:
    CodeBlock(): constraint_cnt(0) {}
    void add_constraint() {
        constraint_cnt++;
    }
    void add_downstream(const std::shared_ptr<CodeBlock>& cb) {
        to_signal.insert(cb);
        cb->add_constraint();
    }
};
#endif