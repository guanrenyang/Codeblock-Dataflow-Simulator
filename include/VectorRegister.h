#ifndef VECTORREGISTER_H
#define VECTORREGISTER_H

#include <random>
#include <utility>

#include "common.h"

class VectorRegister;
typedef std::vector<VectorRegister> VectorRegisterFile;
typedef std::array<uint8_t, 128> VectorData;

/* 1024 bit vector register */
class VectorRegister {
public:
    explicit VectorRegister(DataType dtype = DataType::FLOAT32) {
        /* test */
        // uint8_t* data_ptr = data.data();
        // auto* fp32_data_ptr = reinterpret_cast<float*>(data_ptr);
        // for (int i = 0; i < 128/sizeof(float); i++) {
        //     fp32_data_ptr[i] = static_cast<float>(i);
        // }
    }

    uint8_t operator[](const int i) const { return data[i]; }
    uint8_t& operator[](const int i) { return data[i]; }

    VectorData read_reg() const ;
    void write_reg(const VectorData& new_data);

private:
    VectorData data;
};

#endif //VECTORREGISTER_H
