#include "Inst.h"

template <typename T>
static void add_vv(VectorData& dst, const VectorData& src0, const VectorData& src1) {
    int num_element = dst.size() / sizeof(T);
    const T* src0_ptr = reinterpret_cast<const T*>(src0.data());
    const T* src1_ptr = reinterpret_cast<const T*>(src1.data());
    T* dst_ptr = reinterpret_cast<T*>(dst.data());
    for (int i = 0; i < num_element; i++) {
        dst_ptr[i] = src0_ptr[i] + src1_ptr[i];
    }
}

void CalInst::execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory) {
    // TODO: Add other instructions, for now it is vector add
    VectorData dst_data;

    VectorData src0_data = reg[src0].read_reg();
    VectorData src1_data = reg[src1].read_reg();
    switch (opcode) {
        case 0: add_vv<float>(dst_data, src0_data, src1_data); break; // Vector Add of FP32
    }

    reg[dst].write_reg(dst_data);
};

