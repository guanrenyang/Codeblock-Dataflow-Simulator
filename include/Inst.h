#ifndef INST_H
#define INST_H

#include "common.h"

class Router;
class SPM;
class VectorRegister;
class CodeBlock;

typedef std::vector<VectorRegister> VectorRegisterFile;

class Inst : public std::enable_shared_from_this<Inst>{
protected:
    std::shared_ptr<CodeBlock> code_block;
public:
    Inst() {}
    virtual ~Inst() {}
    virtual void execute(VectorRegisterFile& reg, const std::shared_ptr<SPM>& memory, const std::shared_ptr<Router>& router) = 0 ;

    void register_code_block(std::shared_ptr<CodeBlock> code_block) { // A inst must be assgined to a codeblock
        this->code_block = code_block;
    }

    void register_async_inst();
    void remove_async_inst(); 
};

class CalInst final : public Inst {
public:
    int opcode;
    int dst;
    int src0;
    int src1;
    CalInst(int opcode, int dst, int src0, int src1) : opcode(opcode), dst(dst), src0(src0), src1(src1) {}
    ~CalInst() {}
    void execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory, const std::shared_ptr<Router>& router);
};

class LdInst final : public Inst {
public:
    int dst_pe_row;
    int dst_pe_col;
    int dst_reg_idx;
    int addr;
    LdInst(int dst_pe_row, int dst_pe_col, int dst_reg_idx, int dst, uint32_t addr) : dst_pe_row(dst_pe_row), dst_pe_col(dst_pe_col), dst_reg_idx(dst_reg_idx), addr(addr) {}
    void execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory, const std::shared_ptr<Router>& router);
};

class StInst final: public Inst {
public:
    int src;
    int addr;
    StInst(int src, int addr) : src(src), addr(addr) {}
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
    CopyInst(int src_pe_row, int src_pe_col, int src_reg_idx, int dst_pe_row, int dst_pe_col, int dst_reg_idx) : src_pe_row(src_pe_row), src_pe_col(src_pe_col), src_reg_idx(src_reg_idx), dst_pe_row(dst_pe_row), dst_pe_col(dst_pe_col), dst_reg_idx(dst_reg_idx) {}
    void execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory, const std::shared_ptr<Router>& router);
};
class NopInst final: public Inst {
    void execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory, const std::shared_ptr<Router>& router) {};
};

#endif
