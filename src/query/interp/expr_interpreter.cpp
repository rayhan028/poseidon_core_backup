/*
 * Copyright (C) 2019-2021 DBIS Group - TU Ilmenau, All Rights Reserved.
 *
 * This file is part of the Poseidon package.
 *
 * Poseidon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Poseidon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Poseidon. If not, see <http://www.gnu.org/licenses/>.
 */
#include <stack>

#include "expr_interpreter.hpp"

bool int_equal(const query_result& r1, const query_result& r2) {
    int v1 = boost::get<int>(r1);
    int v2 = boost::get<int>(r2);
    return v1 == v2;
}

query_result pop(std::stack<query_result>& st) {
    auto v = st.top();
    st.pop();
    return v;
}

struct filter_visitor : public expression_visitor {
public:
    filter_visitor(graph_db_ptr& gdb, const qr_tuple& tup) : gdb_(gdb) {}
    virtual ~filter_visitor() = default;

    bool result() { auto v = pop(stack_); return boost::get<int>(v) != 0; }

    virtual void visit(int rank, std::shared_ptr<number_token> op) override {
        std::cout << "visit number_token: " << op->value_ << std::endl;
        stack_.push(query_result(op->value_));
    }

    virtual void visit(int rank, std::shared_ptr<key_token> op) override {
        std::cout << "visit key_token: " << op->qr_id_ << ", " << op->key_ << std::endl;
        stack_.push(query_result(42));
        std::cout << "PUSH: " << "???" << std::endl;
    }

    virtual void visit(int rank, std::shared_ptr<str_token> op) override {
        std::cout << "visit str_token" << std::endl;
    }

    virtual void visit(int rank, std::shared_ptr<time_token> op) override {
        std::cout << "visit time_token" << std::endl;        
    }

    virtual void visit(int rank, std::shared_ptr<fct_call> op) override {
        std::cout << "visit fct_call" << std::endl;         
    }

    virtual void visit(int rank, std::shared_ptr<eq_predicate> op) override {
        auto v1 = pop(stack_);
        auto v2 = pop(stack_);
        bool res = false;
        // type handling!
        switch(op->left_->result_type()) {
            case FOP_TYPE::INT:
                res = int_equal(v1, v2);
                break;
            default:
                break;
        }
        std::cout << "EQ PUSH:" << res << std::endl;
        stack_.push(query_result(res ? 1 : 0));
    }
    
    virtual void visit(int rank, std::shared_ptr<le_predicate> op) override {
        std::cout << "visit le_predicate" << std::endl;       
    }

    virtual void visit(int rank, std::shared_ptr<lt_predicate> op) override {
        std::cout << "visit lt_predicate" << std::endl;       
    }

    virtual void visit(int rank, std::shared_ptr<ge_predicate> op) override {}

    virtual void visit(int rank, std::shared_ptr<gt_predicate> op) override {}

    virtual void visit(int rank, std::shared_ptr<and_predicate> op) override {}

    virtual void visit(int rank, std::shared_ptr<or_predicate> op) override {}

    virtual void visit(int rank, std::shared_ptr<call_predicate> op) override {}

private:
    graph_db_ptr gdb_;
    std::stack<query_result> stack_;
};

bool interpret_expression(graph_db_ptr& gdb, expr& ex, const qr_tuple& tup) {
    std::cout << "interpret_expression: " << ex->dump() << std::endl;
    filter_visitor vis(gdb, tup);
    ex->accept(0, vis);
    return vis.result();
}
