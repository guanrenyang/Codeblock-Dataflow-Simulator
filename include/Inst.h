#ifndef INST_H
#define INST_H

#include "common.h"
#include "VectorRegister.h"

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
    VectorData data;
    
    int remaining_cycles;

    CalInst(int opcode, int dst, int src0, int src1, VectorData data = VectorData{}, int cycles = 1) : opcode(opcode), dst(dst), src0(src0), src1(src1), data(data), remaining_cycles(cycles) {}
    ~CalInst() {}
    void execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory, const std::shared_ptr<Router>& router);
};

class LdInst final : public Inst {
public:
    int dst_pe_row;
    int dst_pe_col;
    int dst_reg_idx;
    int addr;
    int interval;
    LdInst(int dst_pe_row, int dst_pe_col, int dst_reg_idx, uint32_t addr, int interval = 1) : dst_pe_row(dst_pe_row), dst_pe_col(dst_pe_col), dst_reg_idx(dst_reg_idx), addr(addr), interval(interval) {}
    void execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory, const std::shared_ptr<Router>& router);
};

class StInst final: public Inst {
public:
    int src_pe_row;
    int src_pe_col;
    int src_reg_idx;
    uint32_t addr;
    int interval;
    StInst(int src_pe_row, int src_pe_col, int src_reg_idx, uint32_t addr, int interval = 1) : src_pe_row(src_pe_row), src_pe_col(src_pe_col), src_reg_idx(src_reg_idx), addr(addr), interval(interval) {}
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

    int simulation_cycles;

    CopyInst(int src_pe_row, int src_pe_col, int src_reg_idx, int dst_pe_row, int dst_pe_col, int dst_reg_idx, int simulation_cycles = 0) : src_pe_row(src_pe_row), src_pe_col(src_pe_col), src_reg_idx(src_reg_idx), dst_pe_row(dst_pe_row), dst_pe_col(dst_pe_col), dst_reg_idx(dst_reg_idx), simulation_cycles(simulation_cycles) {}
    void execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory, const std::shared_ptr<Router>& router);
};

class NopInst final: public Inst {
    void execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory, const std::shared_ptr<Router>& router) {
        finished = true;
    };
};

#endif
