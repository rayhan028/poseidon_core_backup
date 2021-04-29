rship_predicate rpred = [&](relationship &r) {
    auto &src = gdb->node_by_id(r.from_node_id());
    auto &des = gdb->node_by_id(r.to_node_id());
    node::id_t src_uni, des_uni;
    gdb->foreach_from_relationship_of_node(src, ":studyAt", [&](relationship &r) {
    src_uni = r.to_node_id();
    });
    gdb->foreach_from_relationship_of_node(des, ":studyAt", [&](relationship &r) {
    des_uni = r.to_node_id();
    });
    bool a = std::string(gdb->get_string(r.rship_label)) == ":knows";
    bool b = src_uni == des_uni;
    return a && b;
};

rship_weight rweight = [&](relationship &r) {
    auto &src = gdb->node_by_id(r.from_node_id());
    auto &des = gdb->node_by_id(r.to_node_id());
    int src_yr, des_yr;
    gdb->foreach_from_relationship_of_node(src, ":studyAt", [&](relationship &r1) {
    auto descr = gdb->get_rship_description(r1.id());
    src_yr = get_property<int>(descr.properties, std::string("classYear")).value();
    });
    gdb->foreach_from_relationship_of_node(des, ":studyAt", [&](relationship &r2) {
    auto descr = gdb->get_rship_description(r2.id());
    des_yr = get_property<int>(descr.properties, std::string("classYear")).value();
    });
    double w = std::abs(src_yr - des_yr) + 1;
    return w;
};

Limit(20,
    Sort([$1:double DESC, $0:uint64 ASC],
        Project([$2.id:uint64, $4],
            AlgoShortestPath(TOPKWEIGHTED, {2, 3}, rpred, rweight, TRUE,
                Crossjoin(
                    Expand(IN, "Person",
                        ForeachRelationship(TO, ":workAt",
                            Filter($0.name == %organisation,
                                NodeScan("Organisation")
                            )
                        )
                    ),
                    Filter($0.id == %id,
                        NodeScan("Person")
                    )
                )
            )
        )
    )
)