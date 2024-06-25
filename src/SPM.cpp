#include "SPM.h"
#include "common.h"
#include "RoutePackage.h"
#include "Inst.h"
#include "Router.h"

void SPM::read(int pe_row, int pe_col, int reg_idx, uint32_t addr, std::shared_ptr<Inst> inst, std::shared_ptr<Router> router) {
    uint32_t end_addr = addr + sizeof(VectorData);
    if (end_addr > size || addr < 0) {
        std::cout << "SPM read out of size!";
        abort();
    }

    VectorData res;
    for (int i = 0; i < 128; i++) {
        res[i] = data[addr + i];
    }

    std::shared_ptr<RoutePackage> load_data_package = std::make_shared<LoadDataPackage>(pe_row, pe_col, reg_idx, res, inst);
    router->put(pe_row, pe_col, load_data_package);
    inst->register_async_inst();
}

void SPM::write(uint32_t addr, VectorData src) {
    uint32_t end_addr = addr + sizeof(VectorData);
    if (end_addr > size || addr < 0) {
        std::cout << "SPM write out of size!";
        abort();
    }

    for (int i = 0; i < 128; i++) {
        data[addr + i] = src[i];
    }
}