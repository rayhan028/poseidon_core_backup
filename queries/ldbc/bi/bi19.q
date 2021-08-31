auto interaction = [&](const node &n1, const node &n2) {
    auto count = 0;
    gdb->foreach_to_relationship_of_node(n1, ":hasCreator", [&](relationship &r1) {
    auto &comment = gdb->node_by_id(r1.from_node_id());
    if (comment.node_label == gdb->get_code("Comment")) {
        gdb->foreach_from_relationship_of_node(comment, ":replyOf", [&](relationship &r2) {
        auto &msg = gdb->node_by_id(r2.to_node_id());
        if (msg.node_label == gdb->get_code("Post") || msg.node_label == gdb->get_code("Comment")) {
            gdb->foreach_from_relationship_of_node(msg, ":hasCreator", [&](relationship &r3) {
            if (r3.to_node_id() == n2.id())
                count++;
            });
        }
        });
    }
    });
    return count;
};

auto rweight = [&](relationship &r) {
    double w = 0.0;
    auto &src = gdb->node_by_id(r.from_node_id());
    auto &des = gdb->node_by_id(r.to_node_id());
    auto count = interaction(src, des) + interaction(des, src);
    return count == 0 ? 0 : 1 / (double)count;
};

auto rPred = [&](relationship &r) {
    return std::string(gdb->get_string(r.rship_label)) == ":knows";
};

Limit(20,
    Sort([$2:double DESC, $0:uint64 ASC, $1:uint64 ASC],
        Project([$2.id:uint64, $5.id:uint64, $6],
            AlgoShortestPath(WEIGHTED, {2, 5}, rPred, rweight, TRUE,
                Crossjoin(
                    Expand(IN, "Person",
                        ForeachRelationship(TO, ":isLocatedIn",
                            Filter($0.id == %id2,
                                NodeScan("Place")
                            )
                        )
                    ),
                    Expand(IN, "Person",
                        ForeachRelationship(TO, ":isLocatedIn",
                            Filter($0.id == %id2,
                                NodeScan("Place")
                            )
                        )
                    )
                )
            )
        )
    )
)