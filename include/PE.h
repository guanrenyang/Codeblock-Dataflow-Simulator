#ifndef PE_H
#define PE_H

#include "common.h"
#include "Inst.h"
#include "CodeBlock.h"
#include "LocalScheduler.h"

class PE {
public:
    PE() {
        scheduler = std::make_shared<LocalScheduler>();
        reg = std::vector<VectorRegister>(2048);
    }

    void execute_cycle() {
        Inst inst = scheduler->getReadyInstruction();
        inst.execute(reg);
    }; // perform the operations in the current cycle
    /* Fetch the next instruction from the scheduler
     * execute the instruction */

    void copy(); // TODO: design a better copy interface
private:
    std::shared_ptr<LocalScheduler> scheduler;

    /* Hardware resources */
    std::vector<VectorRegister> reg;
};



#endif