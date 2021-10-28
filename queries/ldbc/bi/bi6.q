Limit(100, 
    Sort([$1:uint64 DESC, $0:uint64 ASC],
        Project([$0.id:uint64, $1:uint64],
            GroupBy([$1:node],
                    [sum($2:uint64)],
                GroupBy([$0:node, $1:node],
                        [count($0:node)],
                    Expand(IN, "Person",
                        ForeachRelationship(TO, ":likes",
                            Expand(IN, "Post", "Comment",
                                ForeachRelationship(TO, ":hasCreator",
                                    GroupBy([$4:node, $6:node],
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