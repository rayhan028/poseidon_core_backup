auto joinPred = [&](auto &lv, auto &rv) {
    return boost::get<node *>(lv[0])->id() == boost::get<node *>(rv[0])->id();
});

auto fltrCnt = [&](auto &v) {
    if (v[3].type() == typeid(null_val)) 
        return true;
    return boost::get<uint64_t>(v[3]) <= (uint64_t)boost::get<int>(params[4]);
});

Limit(20, 
    Project([$0.id:uint64, $1, $5],
        Join(HASHJOIN_ON_NODE, {$0, $0},
            Filter(fltrCnt(tuple),
                Outerjoin(joinPred(tuple1, tuple2),
                    GroupBy([$4],
                            [count($0)],
                        Expand(OUT, "Person",
                            ForeachRelationship(FROM, ":hasCreator",
                                Filter($2.creationDate == %date,
                                    Expand(IN, ["Post", "Comment"],
                                        ForeachRelationship(TO, ":hasTag",
                                            Filter($0.name == %tag,
                                                NodeScan("Tag")
                                            )
                                        )
                                    )
                                )
                            )
                        )
                    ),
                    GroupBy([$0],
                            [count($0)],
                        Filter($0.name == %tag,
                            Expand(OUT, "Tag",
                                ForeachRelationship(FROM, ":hasTag",
                                    Filter($5.creationDate == %date,
                                        Expand(IN, ["Post", "Comment"],
                                            ForeachRelationship(TO, ":hasCreator",
                                                ForeachRelationship(ALL, ":hasCreator", $0,
                                                    GroupBy([$4],
                                                            [count($0)],
                                                        Expand(OUT, "Person",
                                                            ForeachRelationship(FROM, ":hasCreator",
                                                                Filter($2.creationDate == %date,
                                                                    Expand(IN, ["Post", "Comment"],
                                                                        ForeachRelationship(TO, ":hasTag",
                                                                            Filter($0.name == %tag,
                                                                                NodeScan("Tag")
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
            ),
            Filter(fltrCnt(tuple),
                Outerjoin(joinPred(tuple1, tuple2),
                    GroupBy([$0],
                            [count($0)],
                        Filter($0.name == %tag,
                            Expand(OUT, "Tag",
                                ForeachRelationship(FROM, ":hasTag",
                                    Filter($5.creationDate == %date,
                                        Expand(IN, ["Post", "Comment"],
                                            ForeachRelationship(TO, ":hasCreator",
                                                ForeachRelationship(ALL, ":hasCreator", $0,
                                                    GroupBy([$4],
                                                            [count($0)],
                                                        Expand(OUT, "Person",
                                                            ForeachRelationship(FROM, ":hasCreator",
                                                                Filter($2.creationDate == %date,
                                                                    Expand(IN, ["Post", "Comment"],
                                                                        ForeachRelationship(TO, ":hasTag",
                                                                            Filter($0.name == %tag,
                                                                                NodeScan("Tag")
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
                    GroupBy([$4],
                            [count($0)],
                        Expand(OUT, "Person",
                            ForeachRelationship(FROM, ":hasCreator",
                                Filter($2.creationDate == %date,
                                    Expand(IN, ["Post", "Comment"],
                                        ForeachRelationship(TO, ":hasTag",
                                            Filter($0.name == %tag,
                                                NodeScan("Tag")
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