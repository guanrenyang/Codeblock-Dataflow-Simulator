#ifndef SPM_H
#define SPM_H

#include "common.h"
#include "VectorRegister.h"

class SPM { // Scratchpad Memory
private:
    uint8_t *data;

public:
    SPM(int size = 6*1024*1024) { // 6MB
        data = new uint8_t[size];
    }
    ~SPM(){
        delete[] data;
    }

    VectorRegister read(uint32_t addr);
    void write(uint32_t addr, VectorRegister data);
};
#endif