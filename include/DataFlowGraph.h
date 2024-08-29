#ifndef DATAFLOW_SIMULATOR_DFGBUILDER_H
#define DATAFLOW_SIMULATOR_DFGBUILDER_H

#include "common.h"
#include "VectorRegister.h"

class CodeBlock;
class Inst;

class DataFlowGraph {
public:
    // create an empty codeblock
    std::shared_ptr<CodeBlock> createCodeBlock();
    void connectCodeBlock(std::shared_ptr<CodeBlock> src, std::shared_ptr<CodeBlock> dst);
    std::shared_ptr<CodeBlock> createDependentCodeBlock(std::shared_ptr<CodeBlock> upstream_code_block); // create a codeblock that depends on the upstream_code_block

    void appendLoad(std::shared_ptr<CodeBlock> code_block, int dst_pe_row, int dst_pe_col, int dst_reg_idx, int spm_addr, int interval = 1);
    void appendStore(std::shared_ptr<CodeBlock> code_block, int src_pe_row, int src_pe_col, int src_reg_idx, int spm_addr, int interval = 1);
    void appendCopy(std::shared_ptr<CodeBlock> code_block, int src_pe_row, int src_pe_col, int src_reg_idx, int dst_pe_row, int dst_pe_col, int dst_reg_idx);
    void appendCal(std::shared_ptr<CodeBlock> code_block, int opcode, int dst, int src0, int src1);
    void appendMovImm(std::shared_ptr<CodeBlock> code_block, int reg_id, VectorData &data);
    void appendMovReg(std::shared_ptr<CodeBlock> code_block, int src, int dst);

    void signal_and_update() {
        signalDownstream();
        updateConstraint();
    }

    bool is_finished();
private:
    std::vector<std::shared_ptr<CodeBlock>> CodeBlock_list; // CodeBlock can be indexed by its subscript
    std::list<std::shared_ptr<Inst>> Inst_list;
    void signalDownstream();
    void updateConstraint();
};
#endif //DATAFLOW_SIMULATOR_DFGBUILDER_H
