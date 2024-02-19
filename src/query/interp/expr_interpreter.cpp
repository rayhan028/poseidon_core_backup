/*
 * Copyright (C) 2019-2024 DBIS Group - TU Ilmenau, All Rights Reserved.
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
#include <regex>

#include "properties.hpp"
#include "expr_interpreter.hpp"

// ----------- query_result::int -----------
inline bool int_not_equal(const query_result& r1, const query_result& r2) {
    return qv_get_int(r1) != qv_get_int(r2);
}

inline bool int_greater_than(const query_result& r1, const query_result& r2) {
    return qv_get_int(r1) > qv_get_int(r2);
}

inline bool int_less_than(const query_result& r1, const query_result& r2) {
    return qv_get_int(r1) < qv_get_int(r2);
}

inline bool uint64_less_than(const query_result& r1, const query_result& r2) {
    return qv_get_uint64(r1) < qv_get_uint64(r2);
}

inline bool uint64_greater_than(const query_result& r1, const query_result& r2) {
    return qv_get_uint64(r1) > qv_get_uint64(r2);
}

// ----------- query_result::double -----------
inline bool double_not_equal(const query_result& r1, const query_result& r2) {
    return qv_get_double(r1) != qv_get_double(r2);
}

inline bool double_greater_than(const query_result& r1, const query_result& r2) {
    return qv_get_double(r1) > qv_get_double(r2);
}

inline bool double_less_than(const query_result& r1, const query_result& r2) {
    return qv_get_double(r1) < qv_get_double(r2);
}

// ----------- query_result::string -----------
inline bool string_not_equal(const query_result& r1, const query_result& r2) {
    return qv_get_string(r1) != qv_get_string(r2);
}

inline bool string_equal(const query_result& r1, const query_result& r2) {
    return qv_get_string(r1) == qv_get_string(r2);
}

inline bool string_greater_than(const query_result& r1, const query_result& r2) {
    return qv_get_string(r1) > qv_get_string(r2);
}

inline bool string_less_than(const query_result& r1, const query_result& r2) {
    return qv_get_string(r1) < qv_get_string(r2);
}

// ----------- query_result -----------
bool equal(const query_result& qr1, const query_result& qr2) {
    // std::cout << "equal: " << qr1.which() << "-" << qr2.which() << std::endl;
    if (qr1.which() == qr2.which()) {
        switch (qr1.which()) {
            case node_ptr_type: // node *
                return qv_get_node(qr1) == qv_get_node(qr2);
            case rship_ptr_type: // relationship *
                return qv_get_relationship(qr1) == qv_get_relationship(qr2);
                break;
            case int_type: // int
                return qv_get_int(qr1) == qv_get_int(qr2);
            case double_type: // double
                return qv_get_double(qr1) == qv_get_double(qr2);
            case string_type: // std::string
                return string_equal(qr1, qr2);
            case uint64_type: // uint64_t
                return qv_get_uint64(qr1) == qv_get_uint64(qr2);
            case ptime_type: 
                return qv_get_ptime(qr1) == qv_get_ptime(qr2);
            default:
                break;
        }
    } 
    else if (qr1.which() == int_type && qr2.which() == uint64_type)
        return (uint64_t)qv_get_int(qr1) == qv_get_uint64(qr2);
    else if (qr1.which() == uint64_type && qr2.which() == int_type)
        return qv_get_uint64(qr1) == (uint64_t)qv_get_int(qr2);
   else if (qr1.which() == double_type && qr2.which() == uint64_type)
        return qv_get_double(qr1) == (double)qv_get_uint64(qr2);
    else if (qr1.which() == uint64_type && qr2.which() == double_type)
        return (double)qv_get_uint64(qr1) == qv_get_double(qr2);
    else if (qr1.which() == double_type && qr2.which() == int_type)
        return qv_get_double(qr1) == (double)qv_get_int(qr2);
    else if (qr1.which() == int_type && qr2.which() == double_type)
        return (double)qv_get_int(qr1) == qv_get_double(qr2);
    return false;
}

bool less_than(const query_result& qr1, const query_result& qr2) {
    if (qr1.which() == qr2.which()) {
        switch (qr1.which()) {
            case node_ptr_type: // node *
            case rship_ptr_type: // relationship *
                break;
            case int_type: // int
                return qv_get_int(qr1) < qv_get_int(qr2);
            case double_type: // double
                return qv_get_double(qr1) < qv_get_double(qr2);
            case string_type: // std::string
                return string_less_than(qr1, qr2);
            case uint64_type: // uint64_t
                return qv_get_uint64(qr1) < qv_get_uint64(qr2);
            case ptime_type:
                return qv_get_ptime(qr1) < qv_get_ptime(qr2);
            default:
                break;
        }
    } 
    else if (qr1.which() == int_type && qr2.which() == uint64_type)
        return (uint64_t)qv_get_int(qr1) <qv_get_uint64(qr2);
    else if (qr1.which() == uint64_type && qr2.which() ==int_type)
        return qv_get_uint64(qr1) < (uint64_t)qv_get_int(qr2);
   else if (qr1.which() == double_type && qr2.which() == uint64_type)
        return qv_get_double(qr1) < (double)qv_get_uint64(qr2);
    else if (qr1.which() == uint64_type && qr2.which() == double_type)
        return (double)qv_get_uint64(qr1) < qv_get_double(qr2);
    else if (qr1.which() == double_type && qr2.which() == int_type)
        return qv_get_double(qr1) < (double)qv_get_int(qr2);
    else if (qr1.which() == int_type && qr2.which() == double_type)
        return (double)qv_get_int(qr1) < qv_get_double(qr2);    
    return false;
}

bool less_or_equal(const query_result& qr1, const query_result& qr2) {
    return less_than(qr1, qr2) || equal(qr1, qr2);
}

bool greater_than(const query_result& qr1, const query_result& qr2) {
    if (qr1.which() == qr2.which()) {
        switch (qr1.which()) {
            case node_ptr_type: // node *
            case rship_ptr_type: // relationship *
                break;
            case int_type: // int
                return qv_get_int(qr1) > qv_get_int(qr2);
            case double_type: // double
               return qv_get_double(qr1) > qv_get_double(qr2);
            case string_type: // std::string
                return string_greater_than(qr1, qr2);
            case uint64_type: // uint64_t
                return qv_get_uint64(qr1) > qv_get_uint64(qr2);
            case ptime_type: // uint64_t
                return qv_get_ptime(qr1) > qv_get_ptime(qr2);
            default:
                break;
        }
    } 
    else if (qr1.which() == int_type && qr2.which() == uint64_type)
        return (uint64_t)qv_get_int(qr1) > qv_get_uint64(qr2);
    else if (qr1.which() == uint64_type && qr2.which() == int_type)
        return qv_get_uint64(qr1) > (uint64_t)qv_get_int(qr2);
    else if (qr1.which() == double_type && qr2.which() == uint64_type)
        return qv_get_double(qr1) > (double)qv_get_uint64(qr2);
    else if (qr1.which() == uint64_type && qr2.which() == double_type)
        return (double)qv_get_uint64(qr1) > qv_get_double(qr2);
    else if (qr1.which() == double_type && qr2.which() == int_type)
        return qv_get_double(qr1) > (double)qv_get_int(qr2);
    else if (qr1.which() == int_type && qr2.which() == double_type)
        return (double)qv_get_int(qr1) > qv_get_double(qr2);
    return false;
}

bool greater_or_equal(const query_result& qr1, const query_result& qr2) {
    return greater_than(qr1, qr2) || equal(qr1, qr2);
}

int int_math_op(expr_op op, int v1, int v2) {
    switch(op) {
    case expr_op::PLUS: return v1 + v2;
    case expr_op::MINUS: return v1 - v2;
    case expr_op::MULT: return v1 * v2;
    case expr_op::DIV: return v1 / v2;
    case expr_op::MOD: return v1 % v2;
    default: return 0;
    }
}

uint64_t uint64_math_op(expr_op op, uint64_t v1, uint64_t v2) {
    switch(op) {
    case expr_op::PLUS: return v1 + v2;
    case expr_op::MINUS: return v1 - v2;
    case expr_op::MULT: return v1 * v2;
    case expr_op::DIV: return v1 / v2;
    case expr_op::MOD: return v1 % v2;
    default: return 0;
    }
}

double double_math_op(expr_op op, double v1, double v2) {
    switch(op) {
    case expr_op::PLUS: return v1 + v2;
    case expr_op::MINUS: return v1 - v2;
    case expr_op::MULT: return v1 * v2;
    case expr_op::DIV: return v1 / v2;
    default: return 0.0;
    }
}

query_result pop(std::stack<query_result>& st) {
    auto v = st.top();
    st.pop();
    return v;
}

struct expr_eval_visitor : public expression_visitor {
public:
    expr_eval_visitor(query_ctx& ctx, const qr_tuple& tup) : ctx_(ctx), tup_(tup) {}
    ~expr_eval_visitor() = default;

    bool bool_result() {  
        auto v = pop(stack_); 
        return qv_get_int(v) != 0; 
    }

    query_result result() {
        auto v = pop(stack_);
        return v;
    }

    virtual void* visit(std::shared_ptr<number_literal> op) override {
        // std::cout << "visit number_literal: " << op->dump() << std::endl;
        if (op->ftype_ == expr_type::INT)
            stack_.push(query_result(op->ivalue_));
        else if (op->ftype_ == expr_type::UINT64)
            stack_.push(query_result(op->lvalue_));
        else
            stack_.push(query_result(op->dvalue_));
        return nullptr;
    }

    virtual void* visit(std::shared_ptr<variable> op) override {
         std::cout << "visit variable: " << op->id_ << ", " << op->pcode_ << " : " << tup_.size() << std::endl;
        auto inp = tup_[op->id_];
        p_item res;
        switch (inp.which()) {
            case node_ptr_type: // node *
            {
                auto nptr = qv_get_node(inp);
                // if key_ is empty then the node is requested ($i:node)
                if (op->pcode_ == UNKNOWN_CODE) {
                     std::cout << "id = " << nptr->id() << std::endl;
                    stack_.push(query_result(nptr));
                }
                else {
                    // otherwise the property value ($i.prop:dtype) which is handled later
                    res = ctx_.gdb_->get_property_value(*nptr, op->pcode_);
                }
                break;
            }
            case rship_ptr_type: // relationship *
            {
                auto rptr = qv_get_relationship(inp);
                // if key_ is empty then the relationship is requested
                if (op->pcode_ == UNKNOWN_CODE)
                    stack_.push(query_result(rptr));
                else
                    // otherwise the property value ($i.prop:dtype) which is handled later
                    res = ctx_.gdb_->get_property_value(*rptr, op->pcode_);
                break;
            }
            case node_descr_type:
            {
                auto nd = qv_get_node_descr(inp);
                if (op->pname_.empty() && op->pcode_ == UNKNOWN_CODE)
                    stack_.push(query_result(nd));
                else {
                    auto ty = op->result_type();
                    switch(ty) {
                    case expr_type::INT:
                        stack_.push(qv_(get_property<int>(nd.properties, op->pname_).value())); break;
                    case expr_type::UINT64:
                        stack_.push(qv_(get_property<uint64_t>(nd.properties, op->pname_).value())); break;
                    case expr_type::DOUBLE:
                        stack_.push(qv_(get_property<double>(nd.properties, op->pname_).value())); break;
                   case expr_type::STRING:
                        stack_.push(qv_(get_property<std::string>(nd.properties, op->pname_).value())); break;
                   case expr_type::DATETIME:
                        stack_.push(qv_(get_property<boost::posix_time::ptime>(nd.properties, op->pname_).value())); break;
                    default:
                        break;
                 }
                }
                break;
            }
            case rship_descr_type:
            {
                auto rd = qv_get_rship_descr(inp);
                if (op->pname_.empty() && op->pcode_ == UNKNOWN_CODE)
                    stack_.push(query_result(rd));
                else {
                    auto ty = op->result_type();
                    switch(ty) {
                    case expr_type::INT:
                        stack_.push(qv_(get_property<int>(rd.properties, op->pname_).value())); break;
                    case expr_type::UINT64:
                        stack_.push(qv_(get_property<uint64_t>(rd.properties, op->pname_).value())); break;
                    case expr_type::DOUBLE:
                        stack_.push(qv_(get_property<double>(rd.properties, op->pname_).value())); break;
                   case expr_type::STRING:
                        stack_.push(qv_(get_property<std::string>(rd.properties, op->pname_).value())); break;
                   case expr_type::DATETIME:
                        stack_.push(qv_(get_property<boost::posix_time::ptime>(rd.properties, op->pname_).value())); break;
                    default:
                        break;
                 }
                }
                break;
            }            
            case uint64_type:
            case int_type:
            case double_type:
            case string_type:
                stack_.push(inp);
                return nullptr;
                break;
            default:
                std::cout << "visit variable ==> " << inp.which() << std::endl;
                // Ooops!!
                break;
        }
        switch (res.typecode()) {
            case p_item::p_int:
                stack_.push(query_result(res.get<int>()));
                break;
            case p_item::p_double:
                stack_.push(query_result(res.get<double>()));
                break;
            case p_item::p_uint64:
                stack_.push(query_result(res.get<uint64_t>()));
                break;
            case p_item::p_ptime:
                stack_.push(query_result(res.get<boost::posix_time::ptime>()));
                break;
            case p_item::p_dcode:
                {
                    auto str = ctx_.gdb_->get_dictionary()->lookup_code(res.get<dcode_t>());
                    stack_.push(query_result(std::string(str)));
                }
                break;
            case p_item::p_unused:
                // node* or relationship*
                break;
            default:
                // spdlog::info("cannot push for #{} : inp={}, res={}", op->id_, inp.which(), res.typecode());
                break;
        }            
        // std::cout << "PUSH: " << res << std::endl;
        return nullptr;
    }

    virtual void* visit(std::shared_ptr<string_literal> op) override {
        stack_.push(query_result(op->str_));
        return nullptr;
    }

    virtual void* visit(std::shared_ptr<time_literal> op) override {
        stack_.push(query_result(op->time_));       
        return nullptr;
    }

    virtual void* visit(std::shared_ptr<func_call> op) override {
         std::cout << "visit func_call: " << op->func_name_ << " : " << op->param_list_.size() << std::endl;

        if (op->param_list_.size() == 1) {
            auto arg = pop(stack_);
            assert(op->func1_ptr_ != nullptr);
             std::cout << "func_call: arg = " << arg << std::endl;
            auto res = op->func1_ptr_(ctx_, arg);
            // std::cout << "func_call: result = " << res << std::endl;
            stack_.push(res);
        }
        else if (op->param_list_.size() == 2) {
            auto arg2 = pop(stack_);
            auto arg1 = pop(stack_);
            assert(op->func2_ptr_ != nullptr);
            auto res = op->func2_ptr_(ctx_, arg1, arg2);
            stack_.push(res);
        }      
        return nullptr;
    }

    virtual void* visit(std::shared_ptr<eq_predicate> op) override {
        // std::cout << "visit eq_predicate: ==..." << std::endl;  
        op->left_->accept(*this);     
        op->right_->accept(*this); 

        if (valid_operands()) {
            auto v2 = pop(stack_);
            auto v1 = pop(stack_);
            bool res = equal(v1, v2);
            // std::cout << "visit eq_predicate: ->" << v1 << " == " << v2 << std::endl;       
            stack_.push(query_result(res ? 1 : 0));
        }
        return nullptr;
    }
    
    virtual void* visit(std::shared_ptr<neq_predicate> op) override {
        // std::cout << "visit neq_predicate: <>" << std::endl;  
        op->left_->accept(*this);     
        op->right_->accept(*this); 

        if (valid_operands()) {
            auto v2 = pop(stack_);
            auto v1 = pop(stack_);
            bool res = ! equal(v1, v2);
            // std::cout << "visit eq_predicate: ->" << res << std::endl;       
            stack_.push(query_result(res ? 1 : 0));
        }
        else
            throw query_processing_error("invalid <> expression");
        return nullptr;
    }
    
    virtual void* visit(std::shared_ptr<regex_predicate> op) override {
        //std::cout << "visit regex_predicate: =~" << std::endl;  
        op->left_->accept(*this);     
        if(stack_.empty())
            throw query_processing_error("invalid =~ expression");

        auto v1 = pop(stack_);

        bool res = std::regex_match(qv_get_string(v1), op->re_) ? 1 : 0;
        //spdlog::info("visit regex_predicate: {} = {}", qv_get_string(v1), res);       
        stack_.push(query_result(res ? 1 : 0));
        return nullptr;
    }

    virtual void* visit(std::shared_ptr<le_predicate> op) override {
        // std::cout << "visit le_predicate: <=" << std::endl;       
        op->left_->accept( *this);     
        op->right_->accept(*this); 

        if (valid_operands()) {
            auto v2 = pop(stack_);
            auto v1 = pop(stack_);
            bool res = less_or_equal(v1, v2);
            stack_.push(query_result(res ? 1 : 0));
        }    
        return nullptr;
    }

    virtual void* visit(std::shared_ptr<lt_predicate> op) override {
        op->left_->accept(*this);     
        op->right_->accept(*this); 

        if (valid_operands()) {
            auto v2 = pop(stack_);
            auto v1 = pop(stack_);
            // std::cout << "visit lt_predicate: <" << v1 << ", " << v2 << std::endl;       
            bool res = less_than(v1, v2);
            stack_.push(query_result(res ? 1 : 0));
        }    
        return nullptr;
    }

    virtual void* visit(std::shared_ptr<ge_predicate> op) override {
        op->left_->accept(*this);     
        op->right_->accept(*this); 

        // std::cout << "visit ge_predicate: >=" << std::endl;              
        if (valid_operands()) {
            auto v2 = pop(stack_);
            auto v1 = pop(stack_);
            bool res = greater_or_equal(v1, v2);
            stack_.push(query_result(res ? 1 : 0));
        }    
        return nullptr;
    }

    virtual void* visit(std::shared_ptr<gt_predicate> op) override {
        op->left_->accept(*this);     
        op->right_->accept( *this); 

        // std::cout << "visit gt_predicate: >" << std::endl; 
        if (valid_operands()) {
            auto v2 = pop(stack_);
            auto v1 = pop(stack_);
            bool res = greater_than(v1, v2);
            stack_.push(query_result(res ? 1 : 0));
        }
        return nullptr;
    }

    virtual void* visit(std::shared_ptr<and_predicate> op) override {
        op->left_->accept(*this);     
        op->right_->accept(*this); 

        if (valid_operands()) {
            auto v2 = pop(stack_);
            auto v1 = pop(stack_);
            bool res = qv_get_int(v1) && qv_get_int(v2);
            stack_.push(query_result(res ? 1 : 0));
        }
        return nullptr;
    }

    virtual void* visit(std::shared_ptr<or_predicate> op) override {
        op->left_->accept(*this);     
        op->right_->accept(*this); 

         if (valid_operands()) {
            auto v2 = pop(stack_);
            auto v1 = pop(stack_);
            bool res = qv_get_int(v1) || qv_get_int(v2);
            stack_.push(query_result(res ? 1 : 0));
        }       
        return nullptr;
    }

    virtual void* visit(std::shared_ptr<math_expression> op) override {
        op->left_->accept(*this);     
        op->right_->accept(*this); 

        if (valid_operands()) {
            auto v2 = pop(stack_);
            auto v1 = pop(stack_);
            query_result res;

            if (v1.which() == v2.which()) {
                switch (v1.which()) {
                case int_type: // int
                    res = query_result(int_math_op(op->fop_, qv_get_int(v1), qv_get_int(v2)));
                    break;
                case double_type: // double
                    res = query_result(double_math_op(op->fop_, qv_get_double(v1), qv_get_double(v2)));
                    break;
                case uint64_type: // uint64_t
                    res = query_result(uint64_math_op(op->fop_, qv_get_uint64(v1), qv_get_uint64(v2)));
                    break;
                default:
                    break;
                }
            } 
            else if (v1.which() == int_type && v2.which() == uint64_type)
                res = query_result(uint64_math_op(op->fop_, (uint64_t)qv_get_int(v1), qv_get_uint64(v2)));
            else if (v1.which() == uint64_type && v2.which() == int_type)
                res = query_result(uint64_math_op(op->fop_, qv_get_uint64(v1), (uint64_t)qv_get_int(v2)));
            else if (v1.which() == double_type && v2.which() == int_type)
                res = query_result(double_math_op(op->fop_, qv_get_double(v1), (double)qv_get_int(v2)));
            else if (v1.which() == int_type && v2.which() == double_type)
                res = query_result(double_math_op(op->fop_, (double)qv_get_int(v1), qv_get_double(v2)));
            else if (v1.which() == double_type && v2.which() == uint64_type)
                res = query_result(double_math_op(op->fop_, qv_get_double(v1), (double)qv_get_uint64(v2)));
            else if (v1.which() == uint64_type && v2.which() == double_type)
                res = query_result(double_math_op(op->fop_, (double)qv_get_uint64(v1), qv_get_double(v2)));
            
            stack_.push(res);
        }
        return nullptr;
    }

private:
    bool valid_operands() {
        if (stack_.size() < 2) {
            // std::cout << "stack_size = " << stack_.size() << std::endl;
            stack_.pop();
            stack_.push(query_result(0));
            return false; 
        }
        return true;
    }

    query_ctx ctx_;
    qr_tuple tup_;
    std::stack<query_result> stack_;
};

bool interpret_bool_expression(query_ctx& ctx, const expr& ex, const qr_tuple& tup) {
    expr_eval_visitor vis(ctx, tup);
    // std::cout << "interpret_expression: " << ex->dump() << std::endl;
    ex->accept(vis);
    return vis.bool_result();
}

query_result interpret_expression(query_ctx& ctx, const expr& ex, const qr_tuple& tup) {
    expr_eval_visitor vis(ctx, tup);
    // std::cout << "interpret_expression: " << ex->dump() << std::endl;
    ex->accept(vis);
    return vis.result();
}