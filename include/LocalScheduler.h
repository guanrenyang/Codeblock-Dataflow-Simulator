#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "common.h"

class CodeBlock;
class Inst;

/* In current design, Scheduler is assgined to each PE */
class LocalScheduler {
public:
    void addCodeBlock(std::shared_ptr<CodeBlock> code_block);
    std::shared_ptr<Inst> getReadyInstruction();
private:
    std::shared_ptr<CodeBlock> current_CodeBlock;

    std::set<std::shared_ptr<CodeBlock>> waiting_CodeBlocks;
    std::list<std::shared_ptr<CodeBlock>> ready_CodeBlocks;
    // The two queues above are exclusive

    void check_waiting_CodeBlocks();
};

#endif //SCHEDULER_H
