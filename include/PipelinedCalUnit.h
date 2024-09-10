#ifndef DATAFLOW_SIMULATOR_PIPELINEDCALUNIT_H
#define DATAFLOW_SIMULATOR_PIPELINEDCALUNIT_H

#include "common.h"

class CalInst;
class TensorCalInst;

class VectorRegister;
typedef std::vector<VectorRegister> VectorRegisterFile;
class SPM;

const int CYCLES_VECTOR_FP32 = 4;
const int CYCLES_VECTOR_FP16 = 2;
const int CYCLES_VECTOR_INT8 = 1;
const int CYCLES_TENSOR_FP16 = 6;
const int CYCLES_TENSOR_INT8 = 4;

class PipelinedUnit{
public:
    virtual void execute_instructions() = 0;
    virtual void evict_finished_instructions() = 0;

    void execute_cycle();
};

class PipelinedCalUnit :public  PipelinedUnit{
public:
    void execute_instructions();
    void evict_finished_instructions();

    bool can_issue(std::shared_ptr<CalInst> inst);
    void issue(std::shared_ptr<CalInst> inst);

    PipelinedCalUnit(std::shared_ptr<VectorRegisterFile> accessable_reg) : accessable_reg(accessable_reg) {
        for (auto& i : fp32_cal_inst) {
            i = nullptr;
        }
        for (auto& i : fp16_cal_inst) {
            i = nullptr;
        }
        for (auto& i : int8_cal_inst) {
            i = nullptr;
        }
    }

    void set_register_file(std::shared_ptr<VectorRegisterFile> reg) {
        accessable_reg = reg;
    }

private:
    std::shared_ptr<VectorRegisterFile> accessable_reg;

    std::array<std::shared_ptr<CalInst>, CYCLES_VECTOR_FP32> fp32_cal_inst;
    std::array<std::shared_ptr<CalInst>, CYCLES_VECTOR_FP16> fp16_cal_inst;
    std::array<std::shared_ptr<CalInst>, CYCLES_VECTOR_INT8> int8_cal_inst;
};

class PipelinedTensorCalUnit :public PipelinedUnit{
public:
    bool can_issue(std::shared_ptr<TensorCalInst> inst);
    void issue(std::shared_ptr<TensorCalInst> inst);

    void execute_instructions();
    void evict_finished_instructions();


    PipelinedTensorCalUnit(std::shared_ptr<VectorRegisterFile> accessable_reg) : accessable_reg(accessable_reg) {
        for (auto& i : fp16_tensor_inst) {
            i = nullptr;
        }
        for (auto& i : int8_tensor_inst) {
            i = nullptr;
        }
    }

    void set_register_file(std::shared_ptr<VectorRegisterFile> reg) {
        accessable_reg = reg;
    }

private:
    std::shared_ptr<VectorRegisterFile> accessable_reg;

    std::array<std::shared_ptr<TensorCalInst>, CYCLES_TENSOR_FP16> fp16_tensor_inst;
    std::array<std::shared_ptr<TensorCalInst>, CYCLES_TENSOR_INT8> int8_tensor_inst;
};

#endif //DATAFLOW_SIMULATOR_PIPELINEDCALUNIT_H
