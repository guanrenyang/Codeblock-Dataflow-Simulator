#ifndef PE_H
#define PE_H

#include "Router.h"
#include "RoutePackage.h"
#include "common.h"
#include "Inst.h"
#include "CodeBlock.h"
#include "LocalScheduler.h"
#include "SPM.h"
#include <memory>
#include <utility>
class PE {
public:
    PE(std::shared_ptr<Router> router_ptr = nullptr, std::shared_ptr<VectorRegisterFile> reg_file = nullptr, std::shared_ptr<SPM> spm_ptr = nullptr)  {
        scheduler = std::make_shared<LocalScheduler>();

        accessable_reg = std::move(reg_file);
        accessable_memory = std::move(spm_ptr);
        accessable_router = std::move(router_ptr);
    }

    void execute_cycle() {
        auto inst = scheduler->getReadyInstruction();
        inst->execute(*accessable_reg, accessable_memory, accessable_router);
    }; // perform the operations in the current cycle

    void add_CodeBlock(std::shared_ptr<CodeBlock> code_block) {
        scheduler->addCodeBlock(code_block);
    }

    void set_memory(std::shared_ptr<SPM> spm_ptr) {
        accessable_memory = std::move(spm_ptr);
    }

    void set_register_file(std::shared_ptr<VectorRegisterFile> reg_file) {
        accessable_reg = std::move(reg_file);
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

    void display_regfile() {
        for (const auto& innerVec : (*accessable_reg)) {
            for (uint8_t num : innerVec.read_reg()) {
                std::cout << static_cast<int>(num) << " ";  // Cast to int for readable output
            }
            std::cout << std::endl;
        }
    }
private:
    // class RouterFunctor {
    // private:
    //     std::shared_ptr<Router> accessable_router;
    // public:
    //     RouterFunctor(int pe_row, int pe_col, std::shared_ptr<Router> router) : src_pe_row(pe_row), src_pe_col(pe_col), accessable_router(std::move(router)) {}
    //     int src_pe_row;
    //     int src_pe_col;
    //     void operator()(std::shared_ptr<RoutePackage> route_package) {
    //         accessable_router->put(src_pe_row, src_pe_col, route_package);
    //     }
    // };

    std::shared_ptr<LocalScheduler> scheduler;

    /* Hardware resources */
    std::shared_ptr<SPM> accessable_memory;
    std::shared_ptr<VectorRegisterFile> accessable_reg;
    std::shared_ptr<Router> accessable_router;

    // RouterFunctor putOnChip;
};
#endif