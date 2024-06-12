#ifndef VECTORREGISTER_H
#define VECTORREGISTER_H

#include "common.h"
/* 1024 bit vector register */
class VectorRegister {
public:
    VectorRegister() {
        data = std::vector<uint8_t>(128, 0);
    }

    uint8_t operator[](const int i) const { return data[i]; }
    uint8_t& operator[](const int i) { return data[i]; }

    // overload +

    void write_reg(const std::vector<uint8_t>& new_data) {
        data = new_data;
    }
    std::vector<uint8_t> read_reg() const {
        return data;
    }

private:
    std::vector<uint8_t> data;
};

#endif //VECTORREGISTER_H
