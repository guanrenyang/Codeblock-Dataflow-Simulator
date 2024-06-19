#include "VectorRegister.h"

VectorRegister::VectorRegister() {
    /* test */
    uint8_t* data_ptr = data.data();
    auto* fp32_data_ptr = reinterpret_cast<float*>(data_ptr);
    for (int i = 0; i < 128/sizeof(float); i++) {
        fp32_data_ptr[i] = static_cast<float>(i);
    }
}

VectorData VectorRegister::read_reg() const{
    return data;
}

void VectorRegister::write_reg(const VectorData& new_data) {
    data = new_data;
}