#ifndef INST_H
#define INST_H

#include <assert.h>
#include <memory>
#include <set>

#include "common.h"
#include "VectorRegister.h"
#include "SPM.h"

class AsyncInstManager;
class Inst : public std::enable_shared_from_this<Inst>{
public:
    Inst() {}
    virtual ~Inst() {}
    virtual void execute(VectorRegisterFile& reg, const std::shared_ptr<SPM>& memory) = 0 ;
    virtual void async_wait(AsyncInstManager& async_inst_manager) {} 
};

class CalInst final : public Inst {
public:
    int opcode;
    int dst;
    int src0;
    int src1;
    CalInst(int opcode, int dst, int src0, int src1) : opcode(opcode), dst(dst), src0(src0), src1(src1) {}
    ~CalInst() {}
    void execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory);
};

class LdInst final : public Inst {
public:
    int dst;
    int addr;
    LdInst(int dst, int addr) : dst(dst), addr(addr) {}
    void execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory);
    void async_wait(std::set<std::shared_ptr<Inst>> waiting_set) {
        waiting_set.insert(shared_from_this());
    } 
};

class StInst final: public Inst {
public:
    int src;
    int addr;
    StInst(int src, int addr) : src(src), addr(addr) {}
    void execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory);
    void async_wait(std::set<std::shared_ptr<Inst>> waiting_set) {
        waiting_set.insert(shared_from_this());
    } 
};

class CopyInst final: public Inst {
    int dst_pe_row;
    int dst_pe_col;
    int reg_idx;
    CopyInst(int dst_pe_row, int dst_pe_col, int reg_idx) : dst_pe_row(dst_pe_row), dst_pe_col(dst_pe_col), reg_idx(reg_idx) {}
    void execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory);
    void async_wait(std::set<std::shared_ptr<Inst>> waiting_set) {
        waiting_set.insert(shared_from_this());
    }
};
class NopInst final: public Inst {
    void execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory) {}
};

#endif
