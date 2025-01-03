#ifndef INST_H
#define INST_H

#include "common.h"
#include "Opcode.h"

class Router;
class SPM;
class VectorRegister;
class CodeBlock;

typedef std::vector<VectorRegister> VectorRegisterFile;

class Inst : public std::enable_shared_from_this<Inst>{
protected:
    std::shared_ptr<CodeBlock> code_block = nullptr;

    /* Instruction-level dependency */
    bool finished = false;
    bool signaled = false;

    int constraint_cnt = 0;
    int constraint_delta = 0;
    std::list<std::shared_ptr<Inst>> to_signal;
    void add_constraint(){
        constraint_cnt++;
    }

public:
    Inst() {}
    virtual ~Inst() {}
    virtual void execute(VectorRegisterFile& reg, const std::shared_ptr<SPM>& memory, const std::shared_ptr<Router>& router) = 0 ;

    void register_code_block(std::shared_ptr<CodeBlock> code_block) { // A inst must be assgined to a codeblock
        this->code_block = code_block;
    }

    void register_async_inst();
    void remove_async_inst();

    /* For sync instruction, all its cycles are finished
     * For async instruction, it gets the ack */
    bool is_finished();
    bool ready(); // Check if the instruction is ready to be executed
    void connect_to(const std::shared_ptr<Inst>& inst);
    void signal_downstream_if_finished();
    void update_constraint();
};

class CalInst final : public Inst {
public:
    int opcode;
    int dst;
    int src0;
    int src1;

    int total_cycles;
    int remaining_cycles;

    CalInst(int opcode, int dst, int src0, int src1) : opcode(opcode), dst(dst), src0(src0), src1(src1) {
        CalInstType type = getCalInstType(opcode);
        switch (type) {
            case CalInstType::FP32:
                total_cycles = remaining_cycles = 4;
                break;
            case CalInstType::FP16:
                total_cycles = remaining_cycles = 2;
                break;
            case CalInstType::INT8:
                total_cycles = remaining_cycles = 1;
                break;
        }
    }
    ~CalInst() {}
    void execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory, const std::shared_ptr<Router>& router);
};

class TensorCalInst final: public Inst {
public:
    int opcode;
    int dst;
    int src0;
    int src1;

    int total_cycles;
    int remaining_cycles;

    TensorCalInst(int opcode, int dst, int src0, int src1) : opcode(opcode), dst(dst), src0(src0), src1(src1) {
        TensorCalInstType type = getTensorCalInstType(opcode);
        switch (type) {
            case TensorCalInstType::FP16_TENSOR:
                total_cycles = remaining_cycles = 6;
                break;
            case TensorCalInstType::INT8_TENSOR:
                total_cycles = remaining_cycles = 4;
                break;
        }
    }
    ~TensorCalInst() {}
    void execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory, const std::shared_ptr<Router>& router);
};

class LdInst final : public Inst {
public:
    int dst_pe_row;
    int dst_pe_col;
    int dst_reg_idx;
    int addr;
    bool is_tensor;
    LdInst(int dst_pe_row, int dst_pe_col, int dst_reg_idx, uint32_t addr, bool is_tensor = false) : dst_pe_row(dst_pe_row), dst_pe_col(dst_pe_col), dst_reg_idx(dst_reg_idx), addr(addr), is_tensor(is_tensor) {}
    void execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory, const std::shared_ptr<Router>& router);
};

class StInst final: public Inst {
public:
    int src_pe_row;
    int src_pe_col;
    int src_reg_idx;
    uint32_t addr;
    bool is_tensor;
    StInst(int src_pe_row, int src_pe_col, int src_reg_idx, uint32_t addr, bool is_tensor = false) : src_pe_row(src_pe_row), src_pe_col(src_pe_col), src_reg_idx(src_reg_idx), addr(addr), is_tensor(is_tensor) {}
    void execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory, const std::shared_ptr<Router>& router);
};

class CopyInst final: public Inst {
public:
    int src_pe_row;
    int src_pe_col;
    int dst_pe_row;
    int dst_pe_col;
    int src_reg_idx;
    int dst_reg_idx;

    bool is_tensor;
    CopyInst(int src_pe_row, int src_pe_col, int src_reg_idx, int dst_pe_row, int dst_pe_col, int dst_reg_idx, int is_tensor = false) : src_pe_row(src_pe_row), src_pe_col(src_pe_col), src_reg_idx(src_reg_idx), dst_pe_row(dst_pe_row), dst_pe_col(dst_pe_col), dst_reg_idx(dst_reg_idx), is_tensor(is_tensor) {}
    void execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory, const std::shared_ptr<Router>& router);
};

class NopInst final: public Inst {
    void execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory, const std::shared_ptr<Router>& router) {
        finished = true;
    };
};

#endif
