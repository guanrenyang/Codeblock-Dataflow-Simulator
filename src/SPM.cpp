#include "SPM.h"
#include "RoutePackage.h"

VectorData SPM::read(uint32_t addr, int interval) {
    //interval是读取的间隔byte
    uint32_t end_addr = addr + sizeof(VectorData)*interval;
    if (end_addr > size || addr < 0) {
        std::cout << "SPM read out of size!";
        abort();
    }

    VectorData res;
    for (int i = 0; i < 128; i++) {
        res[i] = data[addr + i*interval];
    }

    return res;
}

void SPM::write(uint32_t addr, VectorData src, int interval) {
    uint32_t end_addr = addr + sizeof(VectorData)*interval;
    if (end_addr > size || addr < 0) {
        std::cout << "SPM write out of size!";
        abort();
    }
    
    for (int i = 0; i < 128; i++) {
        data[addr + i*interval] = src[i];
    }
}

int SPM::getDelay() {
    return this->delay;
}