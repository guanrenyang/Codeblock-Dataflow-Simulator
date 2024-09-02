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

template <typename T>
static void v_add(VectorData& dst, const VectorData& src0, const VectorData& src1) {
    // src1 is discarded
    int num_element = dst.size() / sizeof(T);
    const T* src0_ptr = reinterpret_cast<const T*>(src0.data());
    T* dst_ptr = reinterpret_cast<T*>(dst.data());
    for (int i = 0; i < num_element; i++) {
        dst_ptr[0] += src0_ptr[i];
    }
    for (int i = 1; i < num_element; i++) {
        dst_ptr[i] = dst_ptr[0];
    }
}

static void fp16_mac_m16_k16_n16(TensorData& dst, const TensorData& src0, const TensorData& src1) {
    int M = 16, K = 16, N = 16;
    const half* src0_ptr = reinterpret_cast<const half*>(src0.data());
    const half* src1_ptr = reinterpret_cast<const half*>(src1.data());
    half* dst_ptr = reinterpret_cast<half*>(dst.data());
    for (int m = 0; m < M; m++) {
        for (int n = 0; n < N; n++) {
            half sum(0.0);
            for (int k = 0; k < K; k++) {
                sum += src0_ptr[m*K + k] * src1_ptr[k*N + n];
            }
            dst_ptr[m*N + n] = sum;
        }
    }
}

void Inst::register_async_inst() {
#ifdef DEBUG
    std::cout << "Registering async inst\n" << std::endl;
#endif
    code_block->add_async_inst(shared_from_this());
}

void Inst::remove_async_inst() {
#ifdef DEBUG
    std::cout << "Remove async inst" << std::endl;
#endif
    code_block->remove_async_inst(shared_from_this());
    finished = true;
}

void CalInst::execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory, const std::shared_ptr<Router>& router) {
    if (remaining_cycles > 1) {
        remaining_cycles--;
        return;
    }
    assert(remaining_cycles == 1);
    remaining_cycles--;

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
        case 8: v_add<float>(dst_data, src0_data, src1_data); break;
    }

    reg[dst].write_reg(dst_data);
    finished = true;
};

void TensorCalInst::execute(VectorRegisterFile &reg, const std::shared_ptr<SPM> &memory, const std::shared_ptr<Router> &router) {
    if (remaining_cycles > 1) {
        remaining_cycles--;
        return;
    }
    assert(remaining_cycles == 1);
    remaining_cycles--;

    TensorData dst_data;
    TensorData src0_data;
    TensorData src1_data;

    // pack 4 vectors into a tensor
    size_t back_inserter = 0;
    for (int i = 0, back_inserter = 0; i < 4; i++, back_inserter+=128) {
        VectorData src0_vec = reg[src0 + i].read_reg();
        VectorData src1_vec = reg[src1 + i].read_reg();
        std::copy(src0_vec.begin(), src0_vec.end(), src0_data.begin()+back_inserter);
        std::copy(src1_vec.begin(), src1_vec.end(), src1_data.begin()+back_inserter);
    }

    // perform the operation
    switch (opcode) {
        case 0: fp16_mac_m16_k16_n16(dst_data, src0_data, src1_data); break;
    }

    // write back the tensor to 4 vector registers
    for (int i = 0; i < 4; i++) {
        VectorData dst_vec;
        std::copy_n(dst_data.begin() + 128 * i, 128, dst_vec.begin());
        reg[dst + i].write_reg(dst_vec);
    }
    finished = true;
}

void CopyInst::execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory, const std::shared_ptr<Router>& router) {
    // Put data on on-chip router
    VectorData src_data = reg[src_reg_idx].read_reg();
    std::shared_ptr<RoutePackage> copy_data_package = std::make_shared<CopyDataPackage>(src_pe_row, src_pe_col, dst_pe_row, dst_pe_col, dst_reg_idx, src_data, shared_from_this());
    router->put(copy_data_package);

    // register the async instruction
    this->register_async_inst();
};

void LdInst::execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory, const std::shared_ptr<Router>& router) {
    if (!is_tensor) {
        router->put(std::make_shared<LoadSignalPackage>(dst_pe_row, dst_pe_col, dst_reg_idx, addr, shared_from_this()));
        this->register_async_inst();
    } else {
        for (int i = 0; i < 4; ++i) {
            router->put(std::make_shared<LoadSignalPackage>(dst_pe_row, dst_pe_col, dst_reg_idx + i, addr + i * 128, shared_from_this()));
            this->register_async_inst();
        }
    }
};

void StInst::execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory, const std::shared_ptr<Router>& router) {
    if (!is_tensor) {
        VectorData store_data = reg[src_reg_idx].read_reg();
        router->put(std::make_shared<StoreDataPackage>(src_pe_row, src_pe_col, addr, store_data,shared_from_this()));
        this->register_async_inst();
    } else {
        for (int i = 0; i < 4; ++i) {
            VectorData store_data = reg[src_reg_idx+i].read_reg();
            router->put(std::make_shared<StoreDataPackage>(src_pe_row, src_pe_col, addr+i*128, store_data,shared_from_this()));
            this->register_async_inst();
        }
    }
};

bool Inst::is_finished() {
    return finished;
}

bool Inst::ready() {
    return constraint_cnt==0;
}

void Inst::connect_to(const std::shared_ptr<Inst>& inst) {
    to_signal.push_back(inst);
    inst->add_constraint();
}

void Inst::signal_downstream_if_finished() {
    if (signaled)
        return;

    if (is_finished()) {
        for (const auto& inst : to_signal) {
            inst->constraint_delta++;
        }
        signaled = true;
    }
}

void Inst::update_constraint() {
    constraint_cnt -= constraint_delta;
    constraint_delta = 0;
}