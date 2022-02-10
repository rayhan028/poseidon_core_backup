#ifndef qop_visitor_hpp_
#define qop_visitor_hpp_

#include <memory>

struct scan_nodes;
struct index_scan;
struct foreach_from_relationship;
struct foreach_variable_from_relationship;
struct foreach_all_from_relationship;
struct foreach_variable_all_from_relationship;
struct foreach_variable_to_relationship;
struct foreach_relationship;
struct expand;
struct is_property;
struct node_has_label;
struct get_from_node;
struct get_to_node;
struct printer;
struct limit_result;
struct nodes_connected;
struct order_by;
struct group_by;
struct distinct_tuples;
struct filter_tuple;
struct qr_tuple_append;
struct union_all_qres;
struct count_result;
struct shortest_path_opr;
struct weighted_shortest_path_opr;
struct k_weighted_shortest_path_opr;
struct csr_data;
struct collect_result;
struct end_pipeline;
struct projection;
struct cross_join;
struct nested_loop_join;
struct hash_join;
struct left_outerjoin;
struct left_outerjoin_on_node;
struct rship_join;
struct left_outerjoin_on_rship;
struct create_node;
struct create_relationship;
struct create_rship_on_join;
struct update_node;
struct detach_node;
struct remove_node;
struct remove_rship;

class qop_visitor {
public:
    virtual ~qop_visitor() = default;

    virtual void visit(std::shared_ptr<scan_nodes> op) { }

    virtual void visit(std::shared_ptr<index_scan> op) { }

    virtual void visit(std::shared_ptr<foreach_relationship> op) { }

    virtual void visit(std::shared_ptr<is_property> op) { }

    virtual void visit(std::shared_ptr<node_has_label> op) { }
    
    virtual void visit(std::shared_ptr<expand> op) { }

    virtual void visit(std::shared_ptr<printer> op) { }

    virtual void visit(std::shared_ptr<limit_result> op) { }

    virtual void visit(std::shared_ptr<nodes_connected> op) { }

    virtual void visit(std::shared_ptr<order_by> op) { }

    virtual void visit(std::shared_ptr<group_by> op) { }

    virtual void visit(std::shared_ptr<distinct_tuples> op) { }

    virtual void visit(std::shared_ptr<filter_tuple> op) { }

    virtual void visit(std::shared_ptr<qr_tuple_append> op) { }

    virtual void visit(std::shared_ptr<union_all_qres> op) { }

    virtual void visit(std::shared_ptr<count_result> op) { }

    virtual void visit(std::shared_ptr<shortest_path_opr> op) { }

    virtual void visit(std::shared_ptr<weighted_shortest_path_opr> op) { }

    virtual void visit(std::shared_ptr<k_weighted_shortest_path_opr> op) { }

    virtual void visit(std::shared_ptr<csr_data> op) { }

    virtual void visit(std::shared_ptr<collect_result> op) { }

    virtual void visit(std::shared_ptr<end_pipeline> op) { }

    virtual void visit(std::shared_ptr<projection> op) { }

    virtual void visit(std::shared_ptr<cross_join> op) { }

    virtual void visit(std::shared_ptr<nested_loop_join> op) { }

    virtual void visit(std::shared_ptr<hash_join> op) { }

    virtual void visit(std::shared_ptr<left_outerjoin> op) { }

    virtual void visit(std::shared_ptr<left_outerjoin_on_node> op) { }

    virtual void visit(std::shared_ptr<rship_join> op) { }

    virtual void visit(std::shared_ptr<left_outerjoin_on_rship> op) { }

    virtual void visit(std::shared_ptr<create_node> op) { }

    virtual void visit(std::shared_ptr<create_relationship> op) { }

    virtual void visit(std::shared_ptr<create_rship_on_join> op) { }

    virtual void visit(std::shared_ptr<update_node> op) { }

    virtual void visit(std::shared_ptr<detach_node> op) { }

    virtual void visit(std::shared_ptr<remove_node> op) { }

    virtual void visit(std::shared_ptr<remove_rship> op) { }

};

#endif