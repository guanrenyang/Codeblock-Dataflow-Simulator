#ifndef INST_H
#define INST_H

#include "common.h"

class Router;
class SPM;
class VectorRegister;
typedef std::vector<VectorRegister> VectorRegisterFile;

// class AsyncInstManager;
class Inst : public std::enable_shared_from_this<Inst>{
public:
    Inst() {}
    virtual ~Inst() {}
    virtual void execute(VectorRegisterFile& reg, const std::shared_ptr<SPM>& memory, const std::shared_ptr<Router>& router) = 0 ;
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
    int dst;
    int addr;
    LdInst(int dst, int addr) : dst(dst), addr(addr) {}
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
    int src_pe_row;
    int src_pe_col;
    int dst_pe_row;
    int dst_pe_col;
    int src_reg_idx;
    int dst_reg_idx;
    CopyInst(int src_pe_row, int src_pe_col, int dst_pe_row, int dst_pe_col, int reg_idx) : src_pe_row(src_pe_row), src_pe_col(src_pe_col), dst_pe_row(dst_pe_row), dst_pe_col(dst_pe_col), dst_reg_idx(reg_idx) {}
    void execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory, const std::shared_ptr<Router>& router);
};
class NopInst final: public Inst {
    void execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory, const std::shared_ptr<Router>& router) {};
};

#endif
