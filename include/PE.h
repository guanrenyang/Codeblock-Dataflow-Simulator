#ifndef PE_H
#define PE_H

#include "Router.h"
#include "RoutePackage.h"
#include "common.h"
#include "Inst.h"
#include "CodeBlock.h"
#include "LocalScheduler.h"
#include "SPM.h"
#include "PipelinedCalUnit.h"
#include <memory>
#include <utility>


class PE {
public:
    PE(std::shared_ptr<Router> router_ptr = nullptr, std::shared_ptr<VectorRegisterFile> reg_file = nullptr, std::shared_ptr<SPM> spm_ptr = nullptr)  {
        scheduler = std::make_shared<LocalScheduler>();

        accessable_reg = std::move(reg_file);
        accessable_memory = std::move(spm_ptr);
        accessable_router = std::move(router_ptr);

        pipelined_cal_unit = std::make_shared<PipelinedCalUnit>(accessable_reg);
        pipelined_tensor_core = std::make_shared<PipelinedTensorCalUnit>(accessable_reg);
    }

    void execute_cycle() {
        load_inst = scheduler->getReadyInstruction<LdInst>();
        load_inst->execute(*accessable_reg, accessable_memory, accessable_router);

        store_inst = scheduler->getReadyInstruction<StInst>();
        store_inst->execute(*accessable_reg, accessable_memory, accessable_router);

        copy_inst = scheduler->getReadyInstruction<CopyInst>();
        copy_inst->execute(*accessable_reg, accessable_memory, accessable_router);

        /* Pipelined Cal Unit */
        if(cal_inst_to_issue == nullptr) {
            cal_inst_to_issue = scheduler->getReadyInstruction<CalInst>();
            if(std::dynamic_pointer_cast<CalInst>(cal_inst_to_issue) == nullptr){
                cal_inst_to_issue = nullptr;
            }
        }

        if(cal_inst_to_issue != nullptr) {
            assert(std::dynamic_pointer_cast<CalInst>(cal_inst_to_issue) != nullptr);
            if(pipelined_cal_unit->can_issue(std::dynamic_pointer_cast<CalInst>(cal_inst_to_issue))) {
                pipelined_cal_unit->issue(std::dynamic_pointer_cast<CalInst>(cal_inst_to_issue));
                cal_inst_to_issue = nullptr;
            }
        }

        pipelined_cal_unit->execute_instructions();
        pipelined_cal_unit->evict_finished_instructions();
        /* End of Pipelined Cal Unit */

        /* Pipelined Tensor Core */
        if(tensor_cal_inst_to_issue == nullptr){
            tensor_cal_inst_to_issue = scheduler->getReadyInstruction<TensorCalInst>();
            if(std::dynamic_pointer_cast<TensorCalInst>(tensor_cal_inst_to_issue) == nullptr){
                tensor_cal_inst_to_issue = nullptr;
            }
        }

        if(tensor_cal_inst_to_issue != nullptr) {
            assert(std::dynamic_pointer_cast<TensorCalInst>(tensor_cal_inst_to_issue) != nullptr);
            if(pipelined_tensor_core->can_issue(std::dynamic_pointer_cast<TensorCalInst>(tensor_cal_inst_to_issue))) {
                pipelined_tensor_core->issue(std::dynamic_pointer_cast<TensorCalInst>(tensor_cal_inst_to_issue));
                tensor_cal_inst_to_issue = nullptr;
            }
        }

        pipelined_tensor_core->execute_instructions();
        pipelined_tensor_core->evict_finished_instructions();
    }; // perform the operations in the current cycle

    void add_CodeBlock(std::shared_ptr<CodeBlock> code_block) {
        scheduler->addCodeBlock(code_block);
    }

    void set_memory(std::shared_ptr<SPM> spm_ptr) {
        accessable_memory = std::move(spm_ptr);
    }

    void set_register_file(std::shared_ptr<VectorRegisterFile> reg_file) {
        accessable_reg = std::move(reg_file);
        pipelined_cal_unit->set_register_file(accessable_reg);
        pipelined_tensor_core->set_register_file(accessable_reg);
    }

    void set_router(std::shared_ptr<Router> router_ptr) {
        accessable_router = std::move(router_ptr);
    }

    void display_reg_as_fp32(int idx) {
        assert(idx < 2048);
        VectorData data = (*accessable_reg)[idx].read_reg();
        auto* fp32_data_ptr = reinterpret_cast<float*>(data.data());
        for (int i=0;i<128/sizeof(float);i++) {
            std::cout << fp32_data_ptr[i] << " ";
        }
        std::cout << std::endl;
    }

    template<typename T>
    void display_reg_as(int idx) {
        assert(idx < 2048);
        VectorData data = (*accessable_reg)[idx].read_reg();
        auto* data_ptr = reinterpret_cast<T*>(data.data());
        for (int i=0;i<128/sizeof(T);i++) {
            std::cout << data_ptr[i] << " ";
        }
        std::cout << std::endl;
    }

    void display_regfile() {
        for (const auto& innerVec : (*accessable_reg)) {
            for (uint8_t num : innerVec.read_reg()) {
                std::cout << static_cast<int>(num) << " ";  // Cast to int for readable output
            }
            std::cout << std::endl;
        }
    }

private:
    std::shared_ptr<Inst> load_inst;
    std::shared_ptr<Inst> store_inst;
    std::shared_ptr<Inst> cal_inst_to_issue;
    std::shared_ptr<Inst> copy_inst;
    std::shared_ptr<Inst> tensor_cal_inst_to_issue;

    std::shared_ptr<Inst> current_inst;
    std::shared_ptr<LocalScheduler> scheduler;

    /* Hardware resources */
    std::shared_ptr<SPM> accessable_memory;
    std::shared_ptr<VectorRegisterFile> accessable_reg;
    std::shared_ptr<Router> accessable_router;

    std::shared_ptr<PipelinedCalUnit> pipelined_cal_unit;
    std::shared_ptr<PipelinedTensorCalUnit> pipelined_tensor_core;
    // RouterFunctor putOnChip;
};
#endif