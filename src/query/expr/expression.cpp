#include "expression.hpp"
#include "func_call_expr.hpp"

std::string expression::op_as_string(expr_op fop) const {
    switch (fop) {
        case expr_op::EQ:
            return "==";
        case expr_op::NEQ:
            return "!=";
        case expr_op::LE:
            return "<=";
        case expr_op::LT:
            return "<";
        case expr_op::GE:
            return ">=";
        case expr_op::GT:
            return ">";
        case expr_op::AND:
            return "&&";
        case expr_op::OR:
            return "||";
        case expr_op::NOT:
            return "!";
        default:
            return "";
    }
}

number_token::number_token(int value) : ivalue_(value) {
    name_ = "INT";
    rtype_ = ftype_ = expr_type::INT;
}

number_token::number_token(uint64_t value) : lvalue_(value) {
    name_ = "UINT64";
    rtype_ = ftype_ = expr_type::UINT64;
}

number_token::number_token(double value) : dvalue_(value) {
    name_ = "DOUBLE";
    rtype_ = ftype_ = expr_type::DOUBLE;
}

std::string number_token::dump() const {
    return ftype_ == expr_type::INT ? std::to_string(ivalue_) : std::to_string(dvalue_);
}

void* number_token::accept(expression_visitor &fep) {
    return fep.visit(shared_from_this());
}

variable::variable(unsigned int id, const std::string& p) : id_(id), pname_(p), pcode_(UNKNOWN_CODE) {
    // TODO
}

variable::variable(unsigned int id, const std::string& p, dcode_t pc) : id_(id), pname_(p), pcode_(pc) {
    // TODO
}

std::string variable::dump() const {
    auto suffix = pname_.empty() ? "" : (std::string(".") + pname_);
    return std::string("$") + std::to_string(id_) + suffix;
}

void* variable::accept(expression_visitor& fep) {
    return fep.visit(shared_from_this());
}

key_token::key_token(unsigned qr_id, std::string key) : key_(key), qr_id_(qr_id) {
    name_ = "KEY";
    ftype_ = expr_type::KEY;
}

std::string key_token::dump() const {
    auto suffix = key_.empty() ? "" : (std::string(".") + key_);
    return std::string("$") + std::to_string(qr_id_) + suffix;
}

void* key_token::accept(expression_visitor &fep) {
    return fep.visit(shared_from_this());
}

time_token::time_token(boost::posix_time::ptime time) : time_(time) {
    name_ = "TIME";
    rtype_ = ftype_ = expr_type::TIME;
}

void* time_token::accept(expression_visitor &fep) {
    return fep.visit(shared_from_this());
}

std::string time_token::dump() const {
    return boost::posix_time::to_simple_string(time_);
}

str_token::str_token(std::string str) : str_(str) {
    name_ = "STR";
    rtype_ = ftype_ = expr_type::STRING;
}

std::string str_token::dump() const {
    return str_;
}

void* str_token::accept(expression_visitor &fep) {
    return fep.visit(shared_from_this());
}

qparam_token::qparam_token(std::string str) : str_(str) {
    name_ = "QPARAM";
    ftype_ = expr_type::STRING; // TODO
}

std::string qparam_token::dump() const {
    return str_;
}

void* qparam_token::accept(expression_visitor &fep) {
    // TODO - should not happen, but replaced before
    return nullptr;
}

fct_call::fct_call(fct_int_t fct) : fct_int_(fct), fct_type_(expr_type::INT) {
    name_ = "FCT";
}

fct_call::fct_call(fct_str_t fct) : fct_str_(fct), fct_type_(expr_type::STRING) {
    name_ = "FCT";
}

fct_call::fct_call(fct_uint_t fct) : fct_uint_(fct), fct_type_(expr_type::UINT64) {
    name_ = "FCT";
}

void* fct_call::accept(expression_visitor &fep) {
    return fep.visit(shared_from_this());
}

std::string fct_call::dump() const {
    return "";
}

std::string func_call::dump() const {
    std::string params;
    for (auto i = 0u; i < param_list_.size(); i++) {
        params += param_list_[i]->dump();
        if (i < param_list_.size()-1)
            params += ", ";
    }
    return func_name_ + "(" + params + ")";
}

void* func_call::accept(expression_visitor &fep) {
    for (auto& p : param_list_)    
        p->accept(fep);
    return fep.visit(shared_from_this());
}
