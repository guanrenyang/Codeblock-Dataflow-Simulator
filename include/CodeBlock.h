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

        if (!inst_stream.empty()) {
            inst_stream.back()->connect_to(inst_ptr);
        }
        inst_stream.push_back(inst_ptr);
    }

    /* CodeBlock states */
    bool is_finished() { // Non-empty codeblock can't be released by PE
        return inst_stream.empty() && waiting_async_inst.empty();
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
        for (auto it = waiting_async_inst.begin(); it != waiting_async_inst.end(); ++it) {
            if (*it == inst_ptr) {
                waiting_async_inst.erase(it);
                break; // 只删除第一个匹配的元素后退出循环
            }
        }
    }

    /* instruction valideness management */
    bool has_valid_instruction() {
        return (!inst_stream.empty()) && (inst_stream.front()->ready());
    }

    std::shared_ptr<Inst> popInstruction() {
        assert(has_valid_instruction());

        std::shared_ptr<Inst> return_inst = inst_stream.front();
        inst_stream.pop_front();

        return return_inst;
    }

    std::shared_ptr<Inst> frontInstruction() {
        assert(has_valid_instruction());

        return inst_stream.front();
    }

    void signal_downstream_if_finished() {
        if (signaled)
            return;

        if (is_finished()) {
            for (const auto& cb : to_signal) {
                cb->constraint_delta++;
            }
            signaled = true;
        }
    }
    
    void update_constraint(){
        constraint_cnt -= constraint_delta;
        constraint_delta = 0;
    }

private:
    std::list<std::shared_ptr<Inst>> inst_stream;
    std::list<std::shared_ptr<Inst>> waiting_async_inst;

    bool signaled = false;
    /* Inter-CodeBlock constraints*/
    int constraint_cnt;
    int constraint_delta; // update at the entry of the next cycle
    std::set<std::shared_ptr<CodeBlock>> to_signal; // downstream CodeBlocks
    
    void add_constraint() {
        constraint_cnt++;
    }
};

#endif