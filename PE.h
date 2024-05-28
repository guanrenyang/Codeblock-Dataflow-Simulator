#ifndef PE_H
#define PE_H

#include "common.h"
#include "CodeBlock.h"
#include "LocalScheduler.h"

class PE {
public:
    PE() {
        scheduler = std::make_shared<LocalScheduler>();
        reg = std::vector<VectorRegister>(2048);
    }

    void execute_cycle(); // perform the operations in the current cycle
    /* Fetch the next instruction from the scheduler
     * execute the instruction */

private:
    std::shared_ptr<LocalScheduler> scheduler;

    /* Hardware resources */
    std::vector<VectorRegister> reg;
};



#endif