#include "PipelinedCalUnit.h"
#include "Inst.h"
#include "Opcode.h"

bool PipelinedCalUnit::can_issue(std::shared_ptr<CalInst> inst) {
    if (getCalInstType(inst) == CalInstType::FP32) {
        for (auto i : fp32_cal_inst) {
            if (i == nullptr) {
                return true;
            }
        }
    } else if (getCalInstType(inst) == CalInstType::FP16) {
        for (auto i : fp16_cal_inst) {
            if (i == nullptr) {
                return true;
            }
        }
    } else if (getCalInstType(inst) == CalInstType::INT8) {
        for (auto i : int8_cal_inst) {
            if (i == nullptr) {
                return true;
            }
        }
    }

    return false;
}

void PipelinedCalUnit::issue(std::shared_ptr<CalInst> inst) {
    if (getCalInstType(inst) == CalInstType::FP32) {
        for (auto& i : fp32_cal_inst) {
            if (i == nullptr) {
                i = inst;
                return;
            }
        }
    } else if (getCalInstType(inst) == CalInstType::FP16) {
        for (auto& i : fp16_cal_inst) {
            if (i == nullptr) {
                i = inst;
                return;
            }
        }
    } else if (getCalInstType(inst) == CalInstType::INT8) {
        for (auto& i : int8_cal_inst) {
            if (i == nullptr) {
                i = inst;
                return;
            }
        }
    }
}

void PipelinedCalUnit::execute_instructions() {
    for (auto& i : fp32_cal_inst) {
        if (i != nullptr) {
            i->execute(*accessable_reg, nullptr, nullptr);
        }
    }

    for (auto& i : fp16_cal_inst) {
        if (i != nullptr) {
            i->execute(*accessable_reg, nullptr, nullptr);
        }
    }

    for (auto& i : int8_cal_inst) {
        if (i != nullptr) {
            i->execute(*accessable_reg, nullptr, nullptr);
        }
    }
}

void PipelinedCalUnit::evict_finished_instructions() {
    for (auto& i : fp32_cal_inst) {
        if (i != nullptr && i->is_finished()) {
            i = nullptr;
        }
    }

    for (auto& i : fp16_cal_inst) {
        if (i != nullptr && i->is_finished()) {
            i = nullptr;
        }
    }

    for (auto& i : int8_cal_inst) {
        if (i != nullptr && i->is_finished()) {
            i = nullptr;
        }
    }
}

bool PipelinedTensorCalUnit::can_issue(std::shared_ptr<TensorCalInst> inst) {
    if (getTensorCalInstType(inst) == TensorCalInstType::FP16_TENSOR) {
        for (auto i : fp16_tensor_inst) {
            if (i == nullptr) {
                return true;
            }
        }
    } else if (getTensorCalInstType(inst) == TensorCalInstType::INT8_TENSOR) {
        for (auto i : int8_tensor_inst) {
            if (i == nullptr) {
                return true;
            }
        }
    }

    return false;
}

void PipelinedTensorCalUnit::issue(std::shared_ptr<TensorCalInst> inst) {
    if (getTensorCalInstType(inst) == TensorCalInstType::FP16_TENSOR) {
        for (auto& i : fp16_tensor_inst) {
            if (i == nullptr) {
                i = inst;
                return;
            }
        }
    } else if (getTensorCalInstType(inst) == TensorCalInstType::INT8_TENSOR) {
        for (auto& i : int8_tensor_inst) {
            if (i == nullptr) {
                i = inst;
                return;
            }
        }
    }
}
void PipelinedTensorCalUnit::execute_instructions() {
    for (auto& i : fp16_tensor_inst) {
        if (i != nullptr) {
            i->execute(*accessable_reg, nullptr, nullptr);
        }
    }

    for (auto& i : int8_tensor_inst) {
        if (i != nullptr) {
            i->execute(*accessable_reg, nullptr, nullptr);
        }
    }
}

void  PipelinedTensorCalUnit::evict_finished_instructions() {
    for (auto& i : fp16_tensor_inst) {
        if (i != nullptr && i->is_finished()) {
            i = nullptr;
        }
    }

    for (auto& i : int8_tensor_inst) {
        if (i != nullptr && i->is_finished()) {
            i = nullptr;
        }
    }
}

void PipelinedUnit::execute_cycle() {
    execute_instructions();
    evict_finished_instructions();
}