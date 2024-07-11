#include "DataFlowGraph.h"
#include "CodeBlock.h"
#include "Inst.h"

std::shared_ptr<CodeBlock> DataFlowGraph::createCodeBlock() {
    std::shared_ptr<CodeBlock> code_block = std::make_shared<CodeBlock>();
    CodeBlock_list.push_back(code_block);
    return code_block;
}

void DataFlowGraph::connectCodeBlock(std::shared_ptr<CodeBlock> src, std::shared_ptr<CodeBlock> dst) {
    src->connect_to(dst);
}

void DataFlowGraph::appendCal(std::shared_ptr<CodeBlock> code_block, int opcode, int dst, int src0, int src1) {
    std::shared_ptr<Inst> cal_inst = std::make_shared<CalInst>(opcode, dst, src0, src1);
    code_block->append_instruction(cal_inst);
    Inst_list.push_back(cal_inst);
}

void DataFlowGraph::appendLoad(std::shared_ptr<CodeBlock> code_block, int dst_pe_row, int dst_pe_col, int dst_reg_idx, int spm_addr) {
    std::shared_ptr<Inst> ld_inst = std::make_shared<LdInst>(dst_pe_row, dst_pe_col, dst_reg_idx, spm_addr);
    code_block->append_instruction(ld_inst);
    Inst_list.push_back(ld_inst);
}

void DataFlowGraph::appendStore(std::shared_ptr<CodeBlock> code_block, int src_pe_row, int src_pe_col, int src_reg_idx, int spm_addr) {
    std::shared_ptr<Inst> st_inst = std::make_shared<StInst>(src_pe_row, src_pe_col, src_reg_idx, spm_addr);
    code_block->append_instruction(st_inst);
    Inst_list.push_back(st_inst);
}

void DataFlowGraph::appendCopy(std::shared_ptr<CodeBlock> code_block, int src_pe_row, int src_pe_col, int src_reg_idx, int dst_pe_row, int dst_pe_col, int dst_reg_idx) {
    std::shared_ptr<Inst> copy_inst = std::make_shared<CopyInst>(src_pe_row, src_pe_col, src_reg_idx, dst_pe_row, dst_pe_col, dst_reg_idx);
    code_block->append_instruction(copy_inst);
    Inst_list.push_back(copy_inst);
}

void DataFlowGraph::signalDownstream() {
    for (const auto& inst : Inst_list) {
        inst->signal_downstream_if_finished();
    }
    for (const auto& CodeBlock : CodeBlock_list) {
        CodeBlock->signal_downstream_if_finished();
    }
}

void DataFlowGraph::updateConstraint() {
    for (const auto& inst : Inst_list) {
        inst->update_constraint();
    }
    for (const auto& CodeBlock : CodeBlock_list) {
        CodeBlock->update_constraint();
    }
}
