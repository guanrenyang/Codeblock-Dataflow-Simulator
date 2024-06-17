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

template <typename T>
static void sub_vv(VectorData& dst, const VectorData& src0, const VectorData& src1) {
    int num_element = dst.size() / sizeof(T);
    const T* src0_ptr = reinterpret_cast<const T*>(src0.data());
    const T* src1_ptr = reinterpret_cast<const T*>(src1.data());
    T* dst_ptr = reinterpret_cast<T*>(dst.data());
    for (int i = 0; i < num_element; i++) {
        dst_ptr[i] = src0_ptr[i] - src1_ptr[i];
    }
}

template <typename T>
static void mul_vv(VectorData& dst, const VectorData& src0, const VectorData& src1) {
    int num_element = dst.size() / sizeof(T);
    const T* src0_ptr = reinterpret_cast<const T*>(src0.data());
    const T* src1_ptr = reinterpret_cast<const T*>(src1.data());
    T* dst_ptr = reinterpret_cast<T*>(dst.data());
    for (int i = 0; i < num_element; i++) {
        // Perform multiplication and truncate to T's range
        dst_ptr[i] = src0_ptr[i] * src1_ptr[i];
    }
}

template <typename T>
static void max_vv(VectorData& dst, const VectorData& src0, const VectorData& src1) {
    int num_element = dst.size() / sizeof(T);
    const T* src0_ptr = reinterpret_cast<const T*>(src0.data());
    const T* src1_ptr = reinterpret_cast<const T*>(src1.data());
    T* dst_ptr = reinterpret_cast<T*>(dst.data());
    for (int i = 0; i < num_element; i++) {
        dst_ptr[i] = src0_ptr[i] > src1_ptr[i] ? src0_ptr[i] : src1_ptr[i];
    }
}

template <typename T>
static void min_vv(VectorData& dst, const VectorData& src0, const VectorData& src1) {
    int num_element = dst.size() / sizeof(T);
    const T* src0_ptr = reinterpret_cast<const T*>(src0.data());
    const T* src1_ptr = reinterpret_cast<const T*>(src1.data());
    T* dst_ptr = reinterpret_cast<T*>(dst.data());
    for (int i = 0; i < num_element; i++) {
        dst_ptr[i] = src0_ptr[i] < src1_ptr[i] ? src0_ptr[i] : src1_ptr[i];
    }
}

template <typename T>
static void eq_vv(VectorData& dst, const VectorData& src0, const VectorData& src1) {
    int num_element = dst.size() / sizeof(T);
    const T* src0_ptr = reinterpret_cast<const T*>(src0.data());
    const T* src1_ptr = reinterpret_cast<const T*>(src1.data());
    T* dst_ptr = reinterpret_cast<T*>(dst.data());
    for (int i = 0; i < num_element; i++) {
        dst_ptr[i] = src0_ptr[i] == src1_ptr[i]? 1 : 0;
    }
}

template <typename T>
static void lt_vv(VectorData& dst, const VectorData& src0, const VectorData& src1) {
    int num_element = dst.size() / sizeof(T);
    const T* src0_ptr = reinterpret_cast<const T*>(src0.data());
    const T* src1_ptr = reinterpret_cast<const T*>(src1.data());
    T* dst_ptr = reinterpret_cast<T*>(dst.data());
    for (int i = 0; i < num_element; i++) {
        dst_ptr[i] = src0_ptr[i] < src1_ptr[i] ? 1 : 0;
    }
}

template <typename T>
static void gt_vv(VectorData& dst, const VectorData& src0, const VectorData& src1) {
    int num_element = dst.size() / sizeof(T);
    const T* src0_ptr = reinterpret_cast<const T*>(src0.data());
    const T* src1_ptr = reinterpret_cast<const T*>(src1.data());
    T* dst_ptr = reinterpret_cast<T*>(dst.data());
    for (int i = 0; i < num_element; i++) {
        dst_ptr[i] = src0_ptr[i] > src1_ptr[i] ? 1 : 0;
    }
}

void CalInst::execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory) {
    // TODO: Add other instructions, for now it is vector add
    VectorData dst_data;

    VectorData src0_data = reg[src0].read_reg();
    VectorData src1_data = reg[src1].read_reg();
    switch (opcode) {
        case 0: add_vv<float>(dst_data, src0_data, src1_data); break; // Vector Add of FP32
        case 1: sub_vv<float>(dst_data, src0_data, src1_data); break;
        case 2: mul_vv<float>(dst_data, src0_data, src1_data); break;
        case 3: max_vv<float>(dst_data, src0_data, src1_data); break;
        case 4: min_vv<float>(dst_data, src0_data, src1_data); break;
        case 5: eq_vv<float>(dst_data, src0_data, src1_data); break;
        case 6: lt_vv<float>(dst_data, src0_data, src1_data); break;
        case 7: gt_vv<float>(dst_data, src0_data, src1_data); break;
    }

    reg[dst].write_reg(dst_data);
};

void LdInst::execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory) {
    VectorData dst_data = memory->read(addr);
    reg[dst].write_reg(dst_data);
};

void StInst::execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory) {
    VectorRegister src_reg = reg[src];
    VectorData data_stored = src_reg.read_reg();
    memory->write(addr, data_stored);
};


