#include "expression.hpp"

std::string expression::fop_str(FOP fop) const {
    switch (fop) {
        case FOP::EQ:
            return "==";
        case FOP::NEQ:
            return "!=";
        case FOP::LE:
            return "<=";
        case FOP::LT:
            return "<";
        case FOP::GE:
            return ">=";
        case FOP::GT:
            return ">";
        case FOP::AND:
            return "&&";
        case FOP::OR:
            return "||";
        case FOP::NOT:
            return "!";
        default:
            return "";
    }
}

number_token::number_token(int value) : ivalue_(value) {
    name_ = "INT";
    rtype_ = ftype_ = FOP_TYPE::INT;
}

number_token::number_token(double value) : dvalue_(value) {
    name_ = "DOUBLE";
    rtype_ = ftype_ = FOP_TYPE::DOUBLE;
}
std::string number_token::dump() const {
    return ftype_ == FOP_TYPE::INT ? std::to_string(ivalue_) : std::to_string(dvalue_);
}

void number_token::accept(int rank, expression_visitor &fep) {
    fep.visit(rank, shared_from_this());
}

key_token::key_token(unsigned qr_id, std::string key) : key_(key), qr_id_(qr_id) {
    name_ = "KEY";
    ftype_ = FOP_TYPE::KEY;
}

std::string key_token::dump() const {
    auto suffix = key_.empty() ? "" : (std::string(".") + key_);
    return std::string("$") + std::to_string(qr_id_) + suffix;
}

void key_token::accept(int rank, expression_visitor &fep) {
    fep.visit(rank, shared_from_this());
}

time_token::time_token(boost::posix_time::ptime time) : time_(time) {
    name_ = "TIME";
    rtype_ = ftype_ = FOP_TYPE::TIME;
}

void time_token::accept(int rank, expression_visitor &fep) {
    fep.visit(rank, shared_from_this());
}

std::string time_token::dump() const {
    return boost::posix_time::to_simple_string(time_);
}

str_token::str_token(std::string str) : str_(str) {
    name_ = "STR";
    rtype_ = ftype_ = FOP_TYPE::STRING;
}

std::string str_token::dump() const {
    return str_;
}

void str_token::accept(int rank, expression_visitor &fep) {
    fep.visit(rank, shared_from_this());
}

qparam_token::qparam_token(std::string str) : str_(str) {
    name_ = "QPARAM";
    ftype_ = FOP_TYPE::STRING; // TODO
}

std::string qparam_token::dump() const {
    return str_;
}

void qparam_token::accept(int rank, expression_visitor &fep) {
    // TODO - should not happen, but replaced before
}

fct_call::fct_call(fct_int_t fct) : fct_int_(fct), fct_type_(FOP_TYPE::INT) {
    name_ = "FCT";
}

fct_call::fct_call(fct_str_t fct) : fct_str_(fct), fct_type_(FOP_TYPE::STRING) {
    name_ = "FCT";
}

fct_call::fct_call(fct_uint_t fct) : fct_uint_(fct), fct_type_(FOP_TYPE::UINT64) {
    name_ = "FCT";
}

void fct_call::accept(int rank, expression_visitor &fep) {
    fep.visit(rank, shared_from_this());
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

binary_predicate::binary_predicate(FOP fop, const expr left, const expr right, bool prec, bool is_bool)
        : left_(left), right_(right), fop_(fop), is_bool_(is_bool), prec_(prec) {}

std::string binary_predicate::dump() const {
    auto lhs = (*left_).dump();
    auto rhs = (*right_).dump();
    auto op = fop_str(fop_);
    auto pro = prec_ ? "(" : "";
    auto epi = prec_ ? ")" : "";
    return pro + lhs + op + rhs + epi;
}

eq_predicate::eq_predicate(const expr left, const expr right, bool prec, bool not_)
        : binary_predicate(FOP::EQ, left, right, prec) {
    name_ = "EQ";
    ftype_ = FOP_TYPE::OP;
    rtype_ = FOP_TYPE::BOOL_OP;
    if (not_) fop_ = FOP::NEQ;
}

void eq_predicate::accept(int rank, expression_visitor &fep) {
    left_->accept(rank+1, fep);
    right_->accept(rank+1, fep);
    fep.visit(rank, shared_from_this());
    // TODO: do binary stuff here
}

gt_predicate::gt_predicate(const expr left, const expr right, bool prec, bool not_)
        : binary_predicate(FOP::GT, left, right, prec) {
    name_ = "GT";
    ftype_ = FOP_TYPE::OP;
    rtype_ = FOP_TYPE::BOOL_OP;
}

void gt_predicate::accept(int rank, expression_visitor &fep) {
    left_->accept(rank+1, fep);
    right_->accept(rank+1, fep);
    fep.visit(rank, shared_from_this());
    // TODO: do binary stuff here
}

ge_predicate::ge_predicate(const expr left, const expr right, bool prec, bool not_)
        : binary_predicate(FOP::GE, left, right, prec) {
    name_ = "GE";
    ftype_ = FOP_TYPE::OP;
    rtype_ = FOP_TYPE::BOOL_OP;
}

void ge_predicate::accept(int rank, expression_visitor &fep) {
    left_->accept(rank+1, fep);
    right_->accept(rank+1, fep);
    fep.visit(rank, shared_from_this());
    // TODO: do binary stuff here
}

lt_predicate::lt_predicate(const expr left, const expr right, bool prec, bool not_)
        : binary_predicate(FOP::LT, left, right, prec) {
    name_ = "LT";
    ftype_ = FOP_TYPE::OP;
    rtype_ = FOP_TYPE::BOOL_OP;
}

void lt_predicate::accept(int rank, expression_visitor &fep) {
    left_->accept(rank+1, fep);
    right_->accept(rank+1, fep);
    fep.visit(rank, shared_from_this());
    // TODO: do binary stuff here
}

le_predicate::le_predicate(const expr left, const expr right, bool prec, bool not_)
        : binary_predicate(FOP::LE, left, right, prec) {
    name_ = "LE";
    ftype_ = FOP_TYPE::OP;
    rtype_ = FOP_TYPE::BOOL_OP;
}

void le_predicate::accept(int rank, expression_visitor &fep) {
    left_->accept(rank+1, fep);
    right_->accept(rank+1, fep);
    fep.visit(rank, shared_from_this());
    // TODO: do binary stuff here
}

call_predicate::call_predicate(const expr left, const expr right, bool prec, bool not_)
        : binary_predicate(FOP::EQ, left, right, prec) {
    name_ = "CALL";
    ftype_ = FOP_TYPE::OP;
    if (not_) fop_ = FOP::NEQ;
}

void call_predicate::accept(int rank, expression_visitor &fep) {
    left_->accept(rank+1, fep);
    right_->accept(rank+1, fep);
    fep.visit(rank, shared_from_this());
    // TODO: do binary stuff here
}

and_predicate::and_predicate(const expr left, const expr right, bool prec)
        : binary_predicate(FOP::AND, left, right, prec, true) {
    rtype_ = ftype_ = FOP_TYPE::BOOL_OP;
    name_ = "AND";
}

void and_predicate::accept(int rank, expression_visitor &fep) {
    left_->accept(rank+1, fep);
    right_->accept(rank+1, fep);
    fep.visit(rank, shared_from_this());
    // TODO: do binary stuff here
}

or_predicate::or_predicate(const expr left, const expr right, bool prec)
        : binary_predicate(FOP::OR, left, right, prec, true) {
    rtype_ = ftype_ = FOP_TYPE::BOOL_OP;
    name_ = "OR";
}

void or_predicate::accept(int rank, expression_visitor &fep) {
    left_->accept(rank+1, fep);
    right_->accept(rank+1, fep);
    fep.visit(rank, shared_from_this());
    // TODO: do binary stuff here
}