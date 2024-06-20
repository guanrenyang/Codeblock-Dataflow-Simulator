#ifndef CODEBLOCK_H
#define CODEBLOCK_H

#include "common.h"
#include "Inst.h"
#include <memory>
class CodeBlock : public std::enable_shared_from_this<CodeBlock>{
public:
    CodeBlock(): constraint_cnt(0), constraint_delta(0) {}

    /* CodeBlock management*/
    void connect_to(const std::shared_ptr<CodeBlock>& cb) {
        to_signal.insert(cb);
        cb->add_constraint();
    }

    void append_instruction(const std::shared_ptr<Inst>& inst_ptr) {
        inst_ptr->register_code_block(shared_from_this());
        inst_stream.push_back(inst_ptr);
    }

    /* CodeBlock states */
    bool empty() {
        return inst_stream.empty(); // TODO: && waiting_async_stream.empty();
    }

    // \brief Check if the CodeBlock is ready to be executed
    bool ready() {
        return constraint_cnt==0;
    }

    /* Async inst management*/
    void add_async_inst(std::shared_ptr<Inst> inst_ptr) { // called when the async instruction is issued
        waiting_async_inst.push_back(inst_ptr);
    }

    void remove_async_inst(std::shared_ptr<Inst> inst_ptr) { // called when the async instruction is done
        waiting_async_inst.remove(inst_ptr);
    }
    
    std::shared_ptr<Inst> popInstruction() {
        if (inst_stream.empty())
            return std::make_shared<NopInst>();

        auto inst = inst_stream.back();
        inst_stream.pop_back();
        if (empty()) {
            // signal all downstream CodeBlocks
            for (const auto& cb : to_signal) {
                cb->constraint_delta++;
            }
        }

        return inst;
    }

    void update_constraint(){
        constraint_cnt -= constraint_delta;
        constraint_delta = 0;
    }

private:
    std::vector<std::shared_ptr<Inst>> inst_stream;
    std::list<std::shared_ptr<Inst>> waiting_async_inst;

    /* Inter-CodeBlock constraints*/
    int constraint_cnt;
    int constraint_delta; // update at the entry of the next cycle
    std::set<std::shared_ptr<CodeBlock>> to_signal; // downstream CodeBlocks
    
    void add_constraint() {
        constraint_cnt++;
    }
};

#endif