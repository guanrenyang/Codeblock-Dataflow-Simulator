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

