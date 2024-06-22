#ifndef VECTORREGISTER_H
#define VECTORREGISTER_H

#include "common.h"

class VectorRegister;
typedef std::vector<VectorRegister> VectorRegisterFile;
typedef std::array<uint8_t, 128> VectorData;

/* 1024 bit vector register */
class VectorRegister {
public:
    VectorRegister();

    uint8_t operator[](const int i) const { return data[i]; }
    uint8_t& operator[](const int i) { return data[i]; }

    VectorData read_reg() const ;
    void write_reg(const VectorData& new_data);

private:
    VectorData data;
};

#endif //VECTORREGISTER_H
