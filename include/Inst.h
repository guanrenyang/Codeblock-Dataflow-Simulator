#ifndef INST_H
#define INST_H

#include <assert.h>

#include "common.h"
#include "VectorRegister.h"
class Inst {
public:
    Inst(){}
    virtual ~Inst() = default;
    virtual void execute(VectorRegisterFile& reg) = 0;
};

class CalInst final : public Inst {
public:
    int opcode;
    int dst;
    int src0;
    int src1;
    CalInst(int opcode, int dst, int src0, int src1) : Inst(), opcode(opcode), dst(dst), src0(src0), src1(src1) {}
    ~CalInst() override = default;
    void execute(VectorRegisterFile &reg) override {
        int num_element = reg[src0].num_element();

        const float* src0_reg_ptr = reinterpret_cast<const float*> (reg[src0].read_reg().data());
        const float* src1_reg_ptr = reinterpret_cast<const float*> (reg[src1].read_reg().data());
        std::unique_ptr<float[]> dst_reg_ptr(new float [num_element]);

        for (int i = 0; i < num_element; i++) {
            dst_reg_ptr[i] = src0_reg_ptr[i] + src1_reg_ptr[i];
        }
        reg[dst].write_reg(std::vector<uint8_t>(reinterpret_cast<uint8_t*>(dst_reg_ptr.get()), reinterpret_cast<uint8_t*>(dst_reg_ptr.get()) + num_element * sizeof(float)));
    };

};

class LdInst final : public Inst {
public:
    int dst;
    int addr;
    void execute(VectorRegisterFile &reg) override {}
};

class StInst final: public Inst {
public:
    int src;
    int addr;
    void execute(VectorRegisterFile &reg) override {}
};

class NopInst final: public Inst {
    void execute(VectorRegisterFile &reg) override {}
};

#endif
