#include "defs.hpp"
#include "qresult_iterator.hpp"
#include "qop.hpp"

namespace pj = builtin;

// is2
extern "C" query_result getPost(query_result &res) {
    std::cout << "CALL: getPost" << std::endl;
    return boost::get<std::string>(pj::string_property(res, "imageFile")).empty() ?
        pj::string_property(res, "content") : pj::string_property(res, "imageFile");
}

// is4
extern "C" query_result getMessage(query_result &res) {
    std::cout << "CALL: getMessage" << std::endl;
    return !pj::has_property(res, "imageFile") ?
            pj::string_property(res, "content") :
            boost::get<std::string>(pj::string_property(res, "imageFile")).empty() ?
            pj::string_property(res, "content") : pj::string_property(res, "imageFile");
}

// is7
query_result nodesConnected (graph_db_ptr gdb, query_result &p1, query_result &p2) {
    auto src = boost::get<node *>(p1);
    auto des = boost::get<node *>(p2);
    auto connected = false;
    gdb->foreach_from_relationship_of_node((*src), [&](auto &r) {
    if (r.to_node_id() == des->id())
        connected = true;
    });
    return query_result(connected);
}

extern "C" query_result knowsStatus(query_result &res) {
    return res.type() == typeid(null_val) ?
        query_result(std::string("false")) : query_result(std::string("true"));
}