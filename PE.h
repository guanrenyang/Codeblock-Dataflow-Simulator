#ifndef PE_H
#define PE_H

#include "common.h"
#include "CodeBlock.h"
#include "Scheduler.h"

class VectorRegister;

class PE {
public:
    PE() {
        scheduler = std::make_shared<Scheduler>();
        reg = std::vector<VectorRegister>(2048);
    }

    void execute_cycle(); // perform the operations in the current cycle
    /* Fetch the next instruction from the scheduler
     * execute the instruction */

private:
    std::shared_ptr<Scheduler> scheduler;

    /* Hardware resources */
    std::vector<VectorRegister> reg;
};

/* 1024 bit vector register */
class VectorRegister {
public:
    VectorRegister() {
        data = std::vector<uint8_t>(128, 0);
    }

    uint8_t operator[](const int i) const { return data[i]; }
    uint8_t& operator[](const int i) { return data[i]; }

    void write_reg(const std::vector<uint8_t>& new_data) {
        data = new_data;
    }
    std::vector<uint8_t> read_reg() const {
        return data;
    }

private:
    std::vector<uint8_t> data;
};

#endif