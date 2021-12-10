Limit(100, 
    Sort([$4:uint64 DESC, $0:uint64 ASC],
        Project([$0.id:uint64, $1:uint64, $3:uint64, $5:uint64, $6:uint64],
            Append(udf::score($1:uint64, $3:uint64, $5:uint64),
                HashJoin([$0:node, $0:node],
                    GroupBy([$4:node],
                            [count($0:node)],
                        Expand(IN, "Comment",
                            ForeachRelationship(TO, ":replyOf", $2,
                                Expand(OUT, "Person",
                                    ForeachRelationship(FROM, ":hasCreator",
                                        Expand(IN, "Post", "Comment",
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
                    HashJoin([$0:node, $0:node],
                        GroupBy([$4:node],
                                [count($0:node)],
                            Expand(IN, "Person",
                                ForeachRelationship(TO, ":likes", $2,
                                    Expand(OUT, "Person",
                                        ForeachRelationship(FROM, ":hasCreator",
                                            Expand(IN, "Post", "Comment",
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
                        GroupBy([$4:node],
                                [count($0:node)],
                            Expand(OUT, "Person",
                                ForeachRelationship(FROM, ":hasCreator",
                                    Expand(IN, "Post", "Comment",
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