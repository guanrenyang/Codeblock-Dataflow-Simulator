#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <vector>

#include "CodeBlock.h"

/* In current design, Scheduler is assgined to each PE */
class LocalScheduler {
public:
    void addCodeBlock(std::shared_ptr<CodeBlock> CodeBlock);
    void getReadyInstruction(int instruction_type);
private:
    std::vector<std::shared_ptr<CodeBlock>> waiting_CodeBlocks;
    std::vector<std::shared_ptr<CodeBlock>> ready_CodeBlocks;
};

#endif //SCHEDULER_H
