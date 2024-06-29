#ifndef SPM_H
#define SPM_H

#include "common.h"
#include "VectorRegister.h"
#include "Router.h"
#include "Inst.h"

class SPM { // Scratchpad Memory
private:
    uint8_t *data;
    int size;

public:
    SPM(int size = 6*1024*1024) { // 6MB
        data = new uint8_t[size];
        this->size = size;
        
        // Initialize some data for testing.
        for (int i = 0; i < 128; i++) 
            data[i] = i;
    }
    ~SPM(){
        delete[] data;
    }

    VectorData read(uint32_t addr);
    void write(uint32_t addr, VectorData data);

};
#endif