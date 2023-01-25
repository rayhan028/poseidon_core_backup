#ifndef expression_hpp_
#define expression_hpp_

#include <string>
#include <memory>
#include <iostream>
// #include "jit/p_context.hpp"
#include "filter_visitor.hpp"
// #include "qresult_iterator.hpp"

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

enum class FOP {
    EQ = 0,
    NEQ = 1,
    LE = 2,
    LT = 3,
    GE = 4,
    GT = 5,
    AND = 6,
    OR = 7,
    NOT = 8
};

enum class FOP_TYPE {
    INT = 0,
    DOUBLE = 1,
    STRING = 2,
    DATE = 3,
    TIME = 4,
    OP = 5,
    BOOL_OP = 6,
    KEY = 7,
    UINT64 = 8
};

struct expression;
struct binary_predicate;
struct fep_visitor;
struct number_token;
struct key_token;
struct str_token;
struct qparam_token;
struct time_token;
struct fct_call;
struct func_call;
struct eq_predicate;
struct le_predicate;
struct lt_predicate;
struct ge_predicate;
struct gt_predicate;
struct and_predicate;
struct or_predicate;
struct call_predicate;
using expr = std::shared_ptr<expression>;
using bin_expr = std::shared_ptr<binary_predicate>;

struct expression_visitor {
protected:
    expression_visitor() = default;

public:
    virtual ~expression_visitor() = default;

    virtual void visit(int rank, std::shared_ptr<number_token> op) {}

    virtual void visit(int rank, std::shared_ptr<key_token> op) {}

    virtual void visit(int rank, std::shared_ptr<str_token> op) {}

    virtual void visit(int rank, std::shared_ptr<time_token> op) {}

    virtual void visit(int rank, std::shared_ptr<fct_call> op) {}

    virtual void visit(int rank, std::shared_ptr<func_call> op) {}

    virtual void visit(int rank, std::shared_ptr<eq_predicate> op) {}
    
    virtual void visit(int rank, std::shared_ptr<le_predicate> op) {}

    virtual void visit(int rank, std::shared_ptr<lt_predicate> op) {}

    virtual void visit(int rank, std::shared_ptr<ge_predicate> op) {}

    virtual void visit(int rank, std::shared_ptr<gt_predicate> op) {}

    virtual void visit(int rank, std::shared_ptr<and_predicate> op) {}

    virtual void visit(int rank, std::shared_ptr<or_predicate> op) {}

    virtual void visit(int rank, std::shared_ptr<call_predicate> op) {}
};

struct expression {
    int opd_num;
    std::string name_;
    FOP_TYPE ftype_;
    FOP_TYPE rtype_; // result type - deduced from the operands and the operator

    virtual ~expression() {};

    virtual std::string dump() const = 0;

    virtual void accept(int rank, expression_visitor &vis) = 0;

    std::string fop_str(FOP fop) const;

    FOP_TYPE result_type() const { return rtype_; }
};

struct number_token : public expression, public std::enable_shared_from_this<number_token> {
    int ivalue_;
    double dvalue_;

    number_token(int value = 0);
    number_token(double value);

    std::string dump() const override;

    void accept(int rank, expression_visitor &fep) override;
};

inline expr Int(int value = 0) { return std::make_shared<number_token>(value); }

inline expr Float(double value) { return std::make_shared<number_token>(value); }

struct key_token : public expression, std::enable_shared_from_this<key_token> {
    std::string key_;
    unsigned qr_id_; 

    key_token(unsigned qr_id, std::string key);

    std::string dump() const override;

    void accept(int rank, expression_visitor &fep) override;
};

inline expr Key(unsigned qr_id, std::string value = 0) { return std::make_shared<key_token>(qr_id, value); }

struct str_token : public expression, std::enable_shared_from_this<str_token> {
    std::string str_;

    str_token(std::string str);

    std::string dump() const override;

    void accept(int rank, expression_visitor &fep) override;
};

inline expr Str(std::string value = 0) { return std::make_shared<str_token>(value); }

struct qparam_token : public expression, std::enable_shared_from_this<qparam_token> {
    std::string str_;

    qparam_token(std::string str);

    std::string dump() const override;

    void accept(int rank, expression_visitor &fep) override;
};

inline expr QParam(std::string value = 0) { return std::make_shared<qparam_token>(value); }

struct time_token : public expression, std::enable_shared_from_this<time_token> {
    boost::posix_time::ptime time_;

    time_token(boost::posix_time::ptime time);

    std::string dump() const override;

    void accept(int rank, expression_visitor &fep) override;
};

inline expr Time(boost::posix_time::ptime time) { return std::make_shared<time_token>(time); }

struct fct_call : public expression, std::enable_shared_from_this<fct_call> {
    using fct_int_t = bool (*)(int*);
    using fct_uint_t = bool (*)(uint64_t*);    
    using fct_str_t = bool (*)(char*);  
    fct_int_t fct_int_;
    fct_uint_t fct_uint_;
    fct_str_t fct_str_;
    FOP_TYPE fct_type_;

    fct_call(fct_int_t fct);
    fct_call(fct_uint_t fct);
    fct_call(fct_str_t fct);

    std::string dump() const override;

    void accept(int rank, expression_visitor &fep) override;
};


inline expr Fct(fct_call::fct_int_t fct) { return std::make_shared<fct_call>(fct); }

struct binary_predicate : public expression {
    expr left_;
    expr right_;
    FOP fop_;
    bool is_bool_;
    bool prec_;

    binary_predicate(FOP fop, expr const left = 0, expr const right = 0, bool prec = 0, bool is_bool = 0);

    std::string dump() const override;
};

struct eq_predicate : public binary_predicate, std::enable_shared_from_this<eq_predicate> {
    eq_predicate(expr const left, expr const right, bool prec, bool not_ = false);
    void accept(int rank, expression_visitor &fep) override;
};

struct le_predicate : public binary_predicate, std::enable_shared_from_this<le_predicate> {
    le_predicate(expr const left, expr const right, bool prec, bool not_ = false);

    void accept(int rank, expression_visitor &fep) override;
};

struct lt_predicate : public binary_predicate, std::enable_shared_from_this<lt_predicate> {
    lt_predicate(expr const left, expr const right, bool prec, bool not_ = false);

    void accept(int rank, expression_visitor &fep) override;
};

struct ge_predicate : public binary_predicate, std::enable_shared_from_this<ge_predicate> {
    ge_predicate(expr const left, expr const right, bool prec, bool not_ = false);

    void accept(int rank, expression_visitor &fep) override;
};

struct gt_predicate : public binary_predicate, std::enable_shared_from_this<gt_predicate> {
    gt_predicate(expr const left, expr const right, bool prec, bool not_ = false);

    void accept(int rank, expression_visitor &fep) override;
};

struct call_predicate : public binary_predicate, std::enable_shared_from_this<call_predicate> {
    call_predicate(expr const left, expr const right, bool prec, bool not_ = false);

    void accept(int rank, expression_visitor &fep) override;
};
inline bin_expr Call(expr lhs, expr rhs, bool prec = 0) { return std::make_shared<call_predicate>(lhs, rhs, prec); }

inline bin_expr EQ(expr lhs, expr rhs, bool prec = 0) { return std::make_shared<eq_predicate>(lhs, rhs, prec); }

inline bin_expr NEQ(expr lhs, expr rhs, bool prec = 0) { return std::make_shared<eq_predicate>(lhs, rhs, prec, true); }

inline bin_expr LE(expr lhs, expr rhs, bool prec = 0) { return std::make_shared<le_predicate>(lhs, rhs, prec); }

inline bin_expr LT(expr lhs, expr rhs, bool prec = 0) { return std::make_shared<lt_predicate>(lhs, rhs, prec); }

inline bin_expr GE(expr lhs, expr rhs, bool prec = 0) { return std::make_shared<ge_predicate>(lhs, rhs, prec); }

inline bin_expr GT(expr lhs, expr rhs, bool prec = 0) { return std::make_shared<gt_predicate>(lhs, rhs, prec); }

struct and_predicate : public binary_predicate, std::enable_shared_from_this<and_predicate> {
    and_predicate(expr const left, expr const right, bool prec);

    void accept(int rank, expression_visitor &fep) override;
};

inline bin_expr AND(expr lhs, expr rhs, bool prec = 0) {
    return std::make_shared<and_predicate>(lhs, rhs, prec);
}


struct or_predicate : public binary_predicate, std::enable_shared_from_this<or_predicate> {
    or_predicate(expr const left, expr const right, bool prec);

    void accept(int rank, expression_visitor &fep) override;
};

inline bin_expr OR(expr lhs, expr rhs, bool prec = 0) { return std::make_shared<or_predicate>(lhs, rhs, prec); }

struct grexpr {
    expr lhs;
    bin_expr op;
    expr rhs;
};


#endif 