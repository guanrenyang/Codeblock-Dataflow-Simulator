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
    int delay;

public:
    SPM(int size = 6*1024*1024, int delay = 4) { // 6MB and 4cycles to read/write
        data = new uint8_t[size];
        this->size = size;
        this->delay = delay;
        
        // Initialize some data for testing.
        half* float_data = reinterpret_cast<half*>(data);
        for (int i = 0; i < 512; i++) {
            // float_data[i] = static_cast<float>(i) + 5.0f;
            float_data[i] = half(1.0);
        }
    }
    ~SPM(){
        delete[] data;
    }

    VectorData read(uint32_t addr);
    void write(uint32_t addr, VectorData data);
    int getDelay();

};
#endif