#ifndef PE_H
#define PE_H

#include "common.h"
#include "CodeBlock.h"

class PE {
public:
    // TODO: Wrap the following queues into a codeBlocks pool
    std::vector<std::shared_ptr<CodeBlock>> ready_CodeBlocks;
    std::vector<std::shared_ptr<CodeBlock>> waiting_CodeBlocks;
};
#endif