#ifndef INST_H
#define INST_H

#include <assert.h>

#include "common.h"
#include "VectorRegister.h"
#include "SPM.h"

class Inst {
public:
    Inst() {}
    virtual ~Inst() {}
    virtual void execute(VectorRegisterFile& reg, const std::shared_ptr<SPM>& memory) = 0 ;
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
    void execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory);
};

class StInst final: public Inst {
public:
    int src;
    int addr;
    void execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory);
};

class NopInst final: public Inst {
    void execute(VectorRegisterFile &reg, const std::shared_ptr<SPM>& memory) {}
};

#endif
