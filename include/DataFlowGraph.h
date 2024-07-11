#ifndef DATAFLOW_SIMULATOR_DFGBUILDER_H
#define DATAFLOW_SIMULATOR_DFGBUILDER_H

#include "common.h"

class CodeBlock;
class Inst;

class DataFlowGraph {
public:
    // create an empty codeblock
    std::shared_ptr<CodeBlock> createCodeBlock();
    void connectCodeBlock(std::shared_ptr<CodeBlock> src, std::shared_ptr<CodeBlock> dst);

//    void appendLoad(int codeblock_idx);
//    void appendStore(int codeblock_idx);
//    void appendCopy(int codeblock_idx);
//    void appendCal(int codeblock_idx);

    void appendLoad(std::shared_ptr<CodeBlock> code_block);
    void appendStore(std::shared_ptr<CodeBlock> code_block);
    void appendCopy(std::shared_ptr<CodeBlock> code_block);
    void appendCal(std::shared_ptr<CodeBlock> code_block);

    void signal_and_update() {
        signalDownstream();
        updateConstraint();
    }

private:
    std::vector<std::shared_ptr<CodeBlock>> CodeBlock_list; // CodeBlock can be indexed by its subscript
    std::list<std::shared_ptr<Inst>> Inst_list;
    void signalDownstream();
    void updateConstraint();
};
#endif //DATAFLOW_SIMULATOR_DFGBUILDER_H
