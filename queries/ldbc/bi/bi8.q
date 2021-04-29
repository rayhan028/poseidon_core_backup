auto append100 = [&](auto &v) {
    uint64_t intrst = 100;
    return query_result(intrst);
};

Limit(100, 
    Sort([($1:uint64 + $2:uint64) DESC, $0:uint64 ASC],
        Project([$0.id:uint64, $1, $2],
            GroupBy([$0, $1],
                    [sum($5:uint64)],
                Hashjoin({$3, $0},
                    ForeachRelationship(ALL, ":knows", $0,
                        GroupBy([$0],
                                [sum($1:uint64)],
                            Union(
                                AppendToTuple(append100(tuple),
                                    Project([$2],
                                        Expand(IN, ["Person"],
                                            ForeachRelationship(TO, ":hasInterest",
                                                Filter($0.name == %tag,
                                                    NodeScan("Tag")
                                                )
                                            )
                                        )
                                    )
                                ),
                                GroupBy([$4],
                                        [count($0)],
                                    Expand(OUT, "Person",
                                        ForeachRelationship(FROM, ":hasCreator",
                                            Filter($2.creationDate > %date,
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
                    ),
                    GroupBy([$0],
                            [sum($1:uint64)],
                        Union(
                            AppendToTuple(append100(tuple),
                                Project([$2],
                                    Expand(IN, ["Person"],
                                        ForeachRelationship(TO, ":hasInterest",
                                            Filter($0.name == %tag,
                                                NodeScan("Tag")
                                            )
                                        )
                                    )
                                )
                            ),
                            GroupBy([$4],
                                    [count($0)],
                                Expand(OUT, "Person",
                                    ForeachRelationship(FROM, ":hasCreator",
                                        Filter($2.creationDate > %date,
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