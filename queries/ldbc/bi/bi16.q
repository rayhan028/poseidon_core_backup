Limit(20, 
    Project([$0.id:uint64, $1, $5],
        Join(HASHJOIN_ON_NODE, {$0, $0},
            Filter(udf::fltrCnt(tuple),
                Outerjoin(udf::joinPred(tuple1, tuple2),
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
            Filter(udf::fltrCnt(tuple),
                Outerjoin(udf::joinPred(tuple1, tuple2),
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