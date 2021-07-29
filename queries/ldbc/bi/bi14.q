auto fltrRship = [&](auto &v, int idx, string label) {
    if (v[idx].type() == typeid(null_val)) 
        return false;
    auto r = boost::get<relationship *>(v[idx]);
    return r->rship_label == gdb->get_code(label) ? true : false;
});

auto knowsRship = [&](auto &v) {
    auto r = v[4].type() == typeid(relationship *) ? boost::get<relationship *>(v[4]) :
                v[5].type() == typeid(relationship *) ? boost::get<relationship *>(v[5]) : nullptr;
    return !r ? false : r->rship_label == gdb->get_code(":knows") ? true : false;
});

auto appendScore = [&](auto &v, int score) {
    return query_result((uint64_t)score);
});

Sort([$3:uint64 DESC, $0:uint64 ASC, $1:uint64 ASC],
    Project([$0.id:uint64, $1.id:uint64, $2.name:string, $3],
        GroupBy([$0, $1, $2],
                [sum($3)],
            Union(
                AppendToTuple(appendScore(tuple, 1),
                    Project([$1, $3, $0]
                        Filter(fltrRship(tuple, 6, ":likes"),
                            RshipExists({3, 5},
                                Expand(IN, ["Post", "Comment"],
                                    ForeachRelationship(TO, ":hasCreator", $1,
                                        Crossjoin(
                                            Project([$2, $4],
                                                Expand(IN, "Person",
                                                    ForeachRelationship(TO, ":isLocatedIn",
                                                        Expand(IN, "Place",
                                                            ForeachRelationship(TO, ":isPartOf",
                                                                Filter($0.name == %country,
                                                                    NodeScan("Place")
                                                                )
                                                            )
                                                        )
                                                    )
                                                )
                                            ),
                                            Project([$2, $4],
                                                Expand(IN, "Person",
                                                    ForeachRelationship(TO, ":isLocatedIn",
                                                        Expand(IN, "Place",
                                                            ForeachRelationship(TO, ":isPartOf",
                                                                Filter($0.name == %country,
                                                                    NodeScan("Place")
                                                                )
                                                            )
                                                        )
                                                    )
                                                )
                                            )
                                        )
                                    )
                                )
                            )
                        )
                    )
                ),
                AppendToTuple(appendScore(tuple, 10),
                    Project([$1, $3, $0]
                        Filter(fltrRship(tuple, 6, ":likes"),
                            RshipExists({1, 5},
                                Expand(IN, ["Post", "Comment"],
                                    ForeachRelationship(TO, ":hasCreator", $3,
                                        Crossjoin(
                                            Project([$2, $4],
                                                Expand(IN, "Person",
                                                    ForeachRelationship(TO, ":isLocatedIn",
                                                        Expand(IN, "Place",
                                                            ForeachRelationship(TO, ":isPartOf",
                                                                Filter($0.name == %country,
                                                                    NodeScan("Place")
                                                                )
                                                            )
                                                        )
                                                    )
                                                )
                                            ),
                                            Project([$2, $4],
                                                Expand(IN, "Person",
                                                    ForeachRelationship(TO, ":isLocatedIn",
                                                        Expand(IN, "Place",
                                                            ForeachRelationship(TO, ":isPartOf",
                                                                Filter($0.name == %country,
                                                                    NodeScan("Place")
                                                                )
                                                            )
                                                        )
                                                    )
                                                )
                                            )
                                        )
                                    )
                                )
                            )
                        )
                    )
                ),
                AppendToTuple(appendScore(tuple, 15),
                    Filter(knowsRship(tuple),
                        RshipExists({1, 3},
                            RshipExists({3, 1},
                                Crossjoin(
                                    Project([$2, $4],
                                        Expand(IN, "Person",
                                            ForeachRelationship(TO, ":isLocatedIn",
                                                Expand(IN, "Place",
                                                    ForeachRelationship(TO, ":isPartOf",
                                                        Filter($0.name == %country,
                                                            NodeScan("Place")
                                                        )
                                                    )
                                                )
                                            )
                                        )
                                    ),
                                    Project([$2, $4],
                                        Expand(IN, "Person",
                                            ForeachRelationship(TO, ":isLocatedIn",
                                                Expand(IN, "Place",
                                                    ForeachRelationship(TO, ":isPartOf",
                                                        Filter($0.name == %country,
                                                            NodeScan("Place")
                                                        )
                                                    )
                                                )
                                            )
                                        )
                                    )
                                )
                            )
                        )
                    )
                ),
                AppendToTuple(appendScore(tuple, 1),
                    Project([$1, $3, $0]
                        Filter(fltrRship(tuple, 8, ":replyOf"),
                            RshipExists({7, 5},
                                Expand(IN, ["Post", "Comment"],
                                    ForeachRelationship(TO, ":hasCreator", $3,
                                        Expand(IN, "Comment",
                                            ForeachRelationship(TO, ":hasCreator", $1,
                                                Crossjoin(
                                                    Project([$2, $4],
                                                        Expand(IN, "Person",
                                                            ForeachRelationship(TO, ":isLocatedIn",
                                                                Expand(IN, "Place",
                                                                    ForeachRelationship(TO, ":isPartOf",
                                                                        Filter($0.name == %country,
                                                                            NodeScan("Place")
                                                                        )
                                                                    )
                                                                )
                                                            )
                                                        )
                                                    ),
                                                    Project([$2, $4],
                                                        Expand(IN, "Person",
                                                            ForeachRelationship(TO, ":isLocatedIn",
                                                                Expand(IN, "Place",
                                                                    ForeachRelationship(TO, ":isPartOf",
                                                                        Filter($0.name == %country,
                                                                            NodeScan("Place")
                                                                        )
                                                                    )
                                                                )
                                                            )
                                                        )
                                                    )
                                                )
                                            )
                                        )
                                    )
                                )
                            )
                        )
                    )
                ),
                AppendToTuple(appendScore(tuple, 4),
                    Project([$1, $3, $0]
                        Filter(connectedNodes(tuple),
                            RshipExists({5, 7},
                                Expand(IN, ["Post", "Comment"],
                                    ForeachRelationship(TO, ":hasCreator", $3,
                                        Expand(IN, "Comment",
                                            ForeachRelationship(TO, ":hasCreator", $1,
                                                Crossjoin(
                                                    Project([$2, $4],
                                                        Expand(IN, "Person",
                                                            ForeachRelationship(TO, ":isLocatedIn",
                                                                Expand(IN, "Place",
                                                                    ForeachRelationship(TO, ":isPartOf",
                                                                        Filter($0.name == %country,
                                                                            NodeScan("Place")
                                                                        )
                                                                    )
                                                                )
                                                            )
                                                        )
                                                    ),
                                                    Project([$2, $4],
                                                        Expand(IN, "Person",
                                                            ForeachRelationship(TO, ":isLocatedIn",
                                                                Expand(IN, "Place",
                                                                    ForeachRelationship(TO, ":isPartOf",
                                                                        Filter($0.name == %country,
                                                                            NodeScan("Place")
                                                                        )
                                                                    )
                                                                )
                                                            )
                                                        )
                                                    )
                                                )
                                            )
                                        )
                                    )
                                )
                            )
                        )
                    )
                )
            )
        )
    )
)