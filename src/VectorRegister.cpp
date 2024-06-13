#include "VectorRegister.h"

VectorData VectorRegister::read_reg() const{
    return data;
}

void VectorRegister::write_reg(const VectorData& new_data) {
    data = new_data;
}