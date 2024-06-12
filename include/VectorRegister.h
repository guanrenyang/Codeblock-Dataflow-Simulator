#ifndef VECTORREGISTER_H
#define VECTORREGISTER_H

#include <random>
#include <utility>

#include "common.h"

class VectorRegister;
typedef std::vector<VectorRegister> VectorRegisterFile;

/* 1024 bit vector register */
class VectorRegister {
public:
    explicit VectorRegister(DataType dtype = DataType::FLOAT32) {
        data = std::vector<uint8_t>(128, 0);
        data_type = dtype;
    }

    uint8_t operator[](const int i) const { return data[i]; }
    uint8_t& operator[](const int i) { return data[i]; }

    void write_reg(std::vector<uint8_t> new_data) {
        data = std::move(new_data);
    }

    std::vector<uint8_t> read_reg() const {
        return data;
    }

    /* utils */
    int num_element() const;
    DataType get_data_type() const;

private:
    std::vector<uint8_t> data;
    DataType data_type;
};


#endif //VECTORREGISTER_H
