#include <VectorRegister.h>

int VectorRegister::num_element() const {
    int bytes_per_element = 0;
    switch(data_type) {
        case DataType::INT8:
            bytes_per_element = 1;
        break;
        case DataType::INT16:
            bytes_per_element = 2;
        break;
        case DataType::INT32:
            bytes_per_element = 4;
        break;
        case DataType::FLOAT16:
            bytes_per_element = 2;
        break;
        case DataType::FLOAT32:
            bytes_per_element = 4;
        break;
        case DataType::FLOAT64:
            bytes_per_element = 8;
        break;
        default: ;
    }
    return 128 / bytes_per_element;
}

DataType VectorRegister::get_data_type() const {
    return data_type;
}
