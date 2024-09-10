#include "Opcode.h"
#include "Inst.h"

CalInstType getCalInstType(int opcode) {
    if(Add_vv <= opcode && opcode <= Div_vv){
        return CalInstType::FP32;
    }
    return CalInstType::NOP;
};

CalInstType getCalInstType(std::shared_ptr<CalInst> inst) {
    return getCalInstType(inst->opcode);
};

TensorCalInstType getTensorCalInstType(int opcode) {
    if(opcode == MAC_FP16_M16N16K16){
        return TensorCalInstType::FP16_TENSOR;
    } else if(opcode == MAC_INT8_M16N16K16){
        return TensorCalInstType::INT8_TENSOR;
    }
    return TensorCalInstType::INT8_TENSOR;
}
TensorCalInstType getTensorCalInstType(std::shared_ptr<TensorCalInst> inst) {
    return getTensorCalInstType(inst->opcode);
};