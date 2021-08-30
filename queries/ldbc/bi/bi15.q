auto edge_weight = [&](const node &n1, const node &n2) {
    double w = 0.0;
    gdb->foreach_to_relationship_of_node(n1, ":hasCreator", [&](relationship &r1) {
        auto &msg1 = gdb->node_by_id(r1.from_node_id());
        if (msg1.node_label == gdb->get_code("Post")) {
        gdb->foreach_to_relationship_of_node(msg1, ":containerOf", [&](relationship &r2) {
            auto &forum = gdb->node_by_id(r2.from_node_id());
            if (forum.node_label == gdb->get_code("Forum")) {
            auto forum_descr = gdb->get_node_description(forum.id());
            ptime dt = get_property<ptime>(forum_descr.properties, 
                                                std::string("creationDate")).value();
            ptime gdt1 = boost::get<ptime>(params[2]);
            ptime gdt2 = boost::get<ptime>(params[3]);
            if (gdt1 <= dt && dt <= gdt2) {
                gdb->foreach_to_relationship_of_node(msg1, ":replyOf", [&](relationship &r3) {
                auto &msg2 = gdb->node_by_id(r3.from_node_id());
                if (msg2.node_label == gdb->get_code("Comment")) {
                    gdb->foreach_from_relationship_of_node(msg2, ":hasCreator", [&](relationship &r4) {
                    if (r4.to_node_id() == n2.id())
                        w += 1.0;
                    });
                }
                });
            }
            }
        });
        }
        else if (msg1.node_label == gdb->get_code("Comment")) {
        auto reply_code = gdb->get_code(":replyOf");
        gdb->foreach_variable_from_relationship_of_node(msg1, reply_code, 1, 100, [&](relationship &r2) {
            auto &post = gdb->node_by_id(r2.to_node_id());
            if (post.node_label == gdb->get_code("Post")) {
            gdb->foreach_to_relationship_of_node(post, ":containerOf", [&](relationship &r3) {
                auto &forum = gdb->node_by_id(r3.from_node_id());
                if (forum.node_label == gdb->get_code("Forum")) {
                auto forum_descr = gdb->get_node_description(forum.id());
                ptime dt = get_property<ptime>(forum_descr.properties, 
                                                    std::string("creationDate")).value();
                ptime gdt1 = boost::get<ptime>(params[2]);
                ptime gdt2 = boost::get<ptime>(params[3]);
                if (gdt1 <= dt && dt <= gdt2) {
                    gdb->foreach_to_relationship_of_node(msg1, ":replyOf", [&](relationship &r4) {
                    auto &msg2 = gdb->node_by_id(r4.from_node_id());
                    if (msg1.node_label == gdb->get_code("Comment")) {
                        gdb->foreach_from_relationship_of_node(msg2, ":hasCreator", [&](relationship &r5) {
                        if (r5.to_node_id() == n2.id())
                            w += 0.5;
                        });
                    }
                    });
                }
                }
            });
            }
        });
        }
    });
    return w;
};

auto rshipPred [&](relationship &r) {
    return std::string(gdb->get_string(r.rship_label)) == ":knows";
});

auto appendIds [&](auto &v) {
    auto arr = boost::get<array_t>(v[3]).elems;
    std::vector<offset_t> ids;
    for (auto id : arr) {
        auto &n = gdb->node_by_id(id);
        auto nd = gdb->get_node_description(n.id());
        auto p = get_property<uint64_t>(nd.properties, "id");
        ids.push_back(p.value());
    }
    array_t nids(ids);
    return query_result(nids);
});

auto appendWeight [&](auto &v) {
    double weight = 0.0;
    auto nids = boost::get<array_t>(v[3]).elems;
    for (std::size_t i = 0; i < (nids.size() - 1); i++) {
        auto &n1 = gdb->node_by_id(nids[i]);
        auto &n2 = gdb->node_by_id(nids[i + 1]);
        weight += edge_weight(n1, n2);
        weight += edge_weight(n2, n1);
    }
    return query_result(weight);
});

auto sortedId [&](auto &v) {
    auto a = boost::get<array_t>(q1[4]).elems;
    auto b = boost::get<array_t>(q2[4]).elems;
    auto m = std::mismatch(a.begin(), a.end(), b.begin(), b.end());
    return *(m.first) < *(m.second);
});

Project([$6, $5],
    Sort([$5:double DESC, sortedId(tuple) ASC],
        AppendToTuple(appendWeight(tuple),
            AppendToTuple(appendIds(tuple),
                AlgoShortestPath(UNWEIGHTED, {0, 2}, rshipPred, TRUE, TRUE,
                    Limit(1,
                        Filter($2.id == %id2,
                            Expand(OUT, "Person",
                                ForeachRelationship(FROM, ":knows", {1, 100},
                                    Filter($0.id == %id1,
                                        NodeScan("Person")
                                    )
                                )
                            )
                        )
                    )
            )
        )
    )
)