#ifndef PE_H
#define PE_H

#include "common.h"
#include "Inst.h"
#include "CodeBlock.h"
#include "LocalScheduler.h"
#include "SPM.h"
class PE {
public:
    PE(std::unique_ptr<LocalScheduler> scheduler_ptr, std::shared_ptr<SPM> spm_ptr){
        reg = VectorRegisterFile(2048);
        scheduler = std::move(scheduler_ptr);
        accessable_memory = std::move(spm_ptr);
    }

    void execute_cycle() {
        auto inst = scheduler->getReadyInstruction();
        inst->execute(reg, accessable_memory);
    }; // perform the operations in the current cycle

    /* Fetch the next instruction from the scheduler
     * execute the instruction */
    void copy(); // TODO: design a machesim to simulate the copy operation

    void display_reg_as_fp32(int idx) {
        assert(idx < 2048);
        VectorData data = reg[idx].read_reg();
        auto* fp32_data_ptr = reinterpret_cast<float*>(data.data());
        for (int i=0;i<128/sizeof(float);i++) {
            std::cout << fp32_data_ptr[i] << " ";
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

    std::shared_ptr<SPM> accessable_memory;
    /* Hardware resources */
    std::vector<VectorRegister> reg; };



#endif