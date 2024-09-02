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
        float* float_data = reinterpret_cast<float*>(data);
        for (int i = 0; i < 32; i++) {
            float_data[i] = static_cast<float>(1);
        }
        for (int i = 32; i < 64; i++) {
            float_data[i] = static_cast<float>(2);
        }
        for (int i = 64; i < 1024; i++) {
            float_data[i] = static_cast<float>(i) - 64.0f;
            // float_data[i] = 1.0f;
    }
    ~SPM(){
        delete[] data;
    }

    VectorData read(uint32_t addr);
    void write(uint32_t addr, VectorData data);
    int getDelay();

};
#endif