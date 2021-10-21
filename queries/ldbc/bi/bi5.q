Limit(100, 
    Sort([$4:uint64 DESC, $0:uint64 ASC],
        Project([$0.id:uint64, $1, $3, $5, $6],
            AppendToTuple(udf::score(tuple),
                Hashjoin({$0, $0},
                    GroupBy([$4],
                            [count($0)],
                        Expand(IN, "Comment",
                            ForeachRelationship(TO, ":replyOf", $2,
                                Expand(OUT, "Person",
                                    ForeachRelationship(FROM, ":hasCreator",
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
                    ),
                    Hashjoin({$0, $0},
                        GroupBy([$4],
                                [count($0)],
                            Expand(IN, "Person",
                                ForeachRelationship(TO, ":likes", $2,
                                    Expand(OUT, "Person",
                                        ForeachRelationship(FROM, ":hasCreator",
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
                        ),
                        GroupBy([$4],
                                [count($0)],
                            Expand(OUT, "Person",
                                ForeachRelationship(FROM, ":hasCreator",
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