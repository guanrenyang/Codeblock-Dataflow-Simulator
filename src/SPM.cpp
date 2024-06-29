#include "SPM.h"
#include "common.h"

VectorData SPM::read(uint32_t addr) {
    uint32_t end_addr = addr + sizeof(VectorData);
    if (end_addr > size || addr < 0) {
        std::cout << "SPM read out of size!";
        abort();
    }

    VectorData res;
    for (int i = 0; i < 128; i++) {
        res[i] = data[addr + i];
    }

    return res;
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