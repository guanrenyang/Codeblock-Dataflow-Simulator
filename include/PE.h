#ifndef PE_H
#define PE_H

#include "common.h"
#include "Inst.h"
#include "CodeBlock.h"
#include "LocalScheduler.h"

class PE {
public:
    PE(std::unique_ptr<LocalScheduler> scheduler_ptr){
        reg = VectorRegisterFile(2048);
        scheduler = std::move(scheduler_ptr);
    }

    void execute_cycle() {
        auto inst = scheduler->getReadyInstruction();
        inst->execute(reg);
    }; // perform the operations in the current cycle

    /* Fetch the next instruction from the scheduler
     * execute the instruction */

    void copy(); // TODO: design a better copy interface

    void display_reg(int idx) {
        assert(idx < 2048);
        for (uint8_t elem : reg[idx].read_reg()) {
            std::cout << elem << " ";  // Cast to int for readable output
        }
        std::cout << std::endl;
    }
    void display_regfile() {
        for (const auto& innerVec : reg) {
            for (uint8_t num : innerVec.read_reg()) {
                std::cout << static_cast<int>(num) << " ";  // Cast to int for readable output
            }
            std::cout << std::endl;
        }
    }
private:
    std::unique_ptr<LocalScheduler> scheduler;

    /* Hardware resources */
    std::vector<VectorRegister> reg;
};



#endif