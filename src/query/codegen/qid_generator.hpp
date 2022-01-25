#ifndef POSEIDON_CORE_QID_GENERATOR_HPP
#define POSEIDON_CORE_QID_GENERATOR_HPP
#include "qop_visitor.hpp"

class qid_generator : public qop_visitor {
public:
    std::string qid;

    virtual void visit(std::shared_ptr<scan_nodes> op) {
        qid += "S";
    }

    virtual void visit(std::shared_ptr<foreach_relationship> op) {
        qid += "4";
    }

    virtual void visit(std::shared_ptr<projection> op) {
        qid += "P";
    }

    virtual void visit(std::shared_ptr<expand> op) {
        qid += "E";
    }

    virtual void visit(std::shared_ptr<filter_tuple> op) {
        qid += "F";
    }

    virtual void visit(std::shared_ptr<collect_result> op)  {
        qid += "C";
    }

    virtual void visit(std::shared_ptr<join_op> op) {
        qid += "J";
    }

    virtual void visit(std::shared_ptr<order_by> op) {
        qid += "S";
    }

    virtual void visit(std::shared_ptr<limit_result> op) {
        qid += "L";
    }

    virtual void visit(std::shared_ptr<end_pipeline> op) {
        qid += "N";
    }

    virtual void visit(std::shared_ptr<create_node> op) {
        qid += "A";
    }

    virtual void visit(std::shared_ptr<group_by> op) {
        qid += "G";
    }

    virtual void visit(std::shared_ptr<aggr_op> op) {
        qid += "A";
    }

    virtual void visit(std::shared_ptr<nodes_connected> op) {
        qid += "-";
    }

    virtual void visit(std::shared_ptr<qr_tuple_append> op) {
        qid += "+";
    }
    
    /*virtual void visit(std::shared_ptr<store_op> op) {
        qid += "$";
    }*/
};
#endif //POSEIDON_CORE_QID_GENERATOR_HPP
