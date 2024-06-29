#include "Inst.h"
#include "RoutePackage.h"
#include "SPM.h"
#include "Router.h"
#include "VectorRegister.h"
#include "CodeBlock.h"

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

void Inst::register_async_inst() {
    code_block->add_async_inst(shared_from_this());
}

void Inst::remove_async_inst() {
    code_block->remove_async_inst(shared_from_this());
}

void CalInst::execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory, const std::shared_ptr<Router>& router) {
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

void CopyInst::execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory, const std::shared_ptr<Router>& router) {
    // Put data on on-chip router
    VectorData src_data = reg[src_reg_idx].read_reg();
    std::shared_ptr<RoutePackage> copy_data_package = std::make_shared<CopyDataPackage>(src_pe_row, src_pe_col, dst_pe_row, dst_pe_col, dst_reg_idx, src_data, shared_from_this());
    router->put(copy_data_package);

    // register the async instruction
    this->register_async_inst();
};

void LdInst::execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory, const std::shared_ptr<Router>& router) {
    std::shared_ptr<RoutePackage> load_signal_package = std::make_shared<LoadSignalPackage>(dst_pe_row, dst_pe_col, dst_reg_idx, addr, shared_from_this());
    router->put(load_signal_package);
    this->register_async_inst();
};

void StInst::execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory, const std::shared_ptr<Router>& router) {
    VectorData store_data = reg[src_reg_idx].read_reg();
    std::shared_ptr<RoutePackage> store_data_package = std::make_shared<StoreDataPackage>(src_pe_row, src_pe_col, addr, store_data, shared_from_this());
    router->put(store_data_package);
    this->register_async_inst();
};


