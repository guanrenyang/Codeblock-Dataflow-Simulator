#ifndef INST_H
#define INST_H

#include "common.h"
#include "VectorRegister.h"
class Inst {
public:
    virtual ~Inst() = default;

    virtual void execute(std::vector<VectorRegister>& reg) = 0;
};

class CalInst final : public Inst {
public:
    int opcode;
    int dst;
    int src0;
    int src1;
    void execute(std::vector<VectorRegister> &reg) override;
};

class LdInst final : public Inst {
public:
    int dst;
    int addr;
    void execute(std::vector<VectorRegister> &reg) override;
};
class StInst final: public Inst {
public:
    int src;
    int addr;
    void execute(std::vector<VectorRegister> &reg) override;
};

#endif
