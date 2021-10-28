// is1
auto getDate = [&](query_result &res) {
    return pj::pr_date(res, "birthday");
};

// is2
auto getPost = [&](query_result &res) {
    return boost::get<std::string>(pj::string_property(res, "imageFile")).empty() ?
        pj::string_property(res, "content") : pj::string_property(res, "imageFile");
};

// is4
auto getMessage = [&](query_result &res) {
    return !pj::has_property(res, "imageFile") ?
            pj::string_property(res, "content") :
            boost::get<std::string>(pj::string_property(res, "imageFile")).empty() ?
            pj::string_property(res, "content") : pj::string_property(res, "imageFile");
};

// is7
auto nodesConnected = [&](node *src, node *des) {
    auto connected = false;
    gdb->foreach_from_relationship_of_node((*src), [&](auto &r) {
    if (r.to_node_id() == des->id())
        connected = true;
    });
    return connected;
});

auto knowsStatus = [&](query_result &res) {
    return res.type() == typeid(null_val) ?
        query_result(std::string("false")) : query_result(std::string("true"))
});