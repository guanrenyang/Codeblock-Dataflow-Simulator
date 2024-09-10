#ifndef OPCODE_H
#define OPCODE_H

#include "common.h"

/* Opcodes for Vector Core */
#define Add_vv 0
#define Sub_vv 1
#define Mul_vv 2
#define Max_vv 3
#define Min_vv 4
#define Eq_vv 5
#define Lt_vv 6
#define Gt_vv 7
#define V_add 8
#define Abs_vv 9
#define Max_mask_vv 10
#define Div_vv 11

/* Opcodes for Tensor Core */
#define MAC_FP16_M16N16K16 0
#define MAC_INT8_M16N16K16 1

enum class CalInstType : int {
    NOP = -1,
    FP32 = 0,
    FP16 = 1,
    INT8 = 2,
};

enum class TensorCalInstType: int{
    FP16_TENSOR = 0,
    INT8_TENSOR = 1,
};

class CalInst;
class TensorCalInst;

CalInstType getCalInstType(std::shared_ptr<CalInst> inst) ;
CalInstType getCalInstType(int opcode) ;
TensorCalInstType getTensorCalInstType(std::shared_ptr<TensorCalInst> inst);
TensorCalInstType getTensorCalInstType(int opcode);
#endif