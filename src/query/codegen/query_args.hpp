#ifndef POSEIDON_CORE_ARG_EXTRACTOR_HPP
#define POSEIDON_CORE_ARG_EXTRACTOR_HPP
#include "qop_visitor.hpp"

class arg_extractor : public qop_visitor {
    arg_builder ab_;
    unsigned arg_cnt_;

    std::vector<base_joiner*> joiner_list_;

public:
    arg_extractor() : arg_cnt_(1) {}

    virtual void visit(std::shared_ptr<scan_nodes> op) override {
       auto s_op = std::dynamic_pointer_cast<scan_nodes>(op);
       ab_.arg(arg_cnt_++, s_op->label);
    }

    virtual void visit(std::shared_ptr<foreach_relationship> op) override {
       auto fe_op = std::dynamic_pointer_cast<foreach_relationship>(op);
       ab_.arg(arg_cnt_++, fe_op->label);
    }

    virtual void visit(std::shared_ptr<expand> op) override {
        auto exp_op = std::dynamic_pointer_cast<expand>(op);
        if(!exp_op->label.empty())
                ab_.arg(arg_cnt_, exp_op->label);
        arg_cnt_++;
    }

    virtual void visit(std::shared_ptr<node_has_label> op) override {
        auto hl_op = std::dynamic_pointer_cast<node_has_label>(op);
        ab_.arg(arg_cnt_++, hl_op->label);
    }

    virtual void visit(std::shared_ptr<collect_result> op) override {
        auto cop = std::dynamic_pointer_cast<collect_result>(op);
        ab_.arg(arg_cnt_++, &cop->results_);
    }

    virtual void visit(std::shared_ptr<end_pipeline> op) override {
        auto cop = std::dynamic_pointer_cast<end_pipeline>(op);
        //TODO: create joiner 
    }

    virtual void visit(std::shared_ptr<cross_join> op) override {
        auto last_joiner = joiner_list_.back();
        ab_.arg(arg_cnt_++, last_joiner);
    }

    arg_builder & get_args() { return ab_; }

};
#endif //POSEIDON_CORE_QID_GENERATOR_HPP
