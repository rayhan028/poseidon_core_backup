Limit(100, 
    Sort([udf::add($1:uint64, $2:uint64) DESC, $0:uint64 ASC],
        Project([$0.id:uint64, $1:uint64, $2:uint64],
            GroupBy([$0:node, $1:uint64],
                    [sum($5:uint64)],
                HashJoin([$3:node, $0:node],
                    ForeachRelationship(ALL, ":knows", $0,
                        GroupBy([$0:node],
                                [sum($1:uint64)],
                            Union(
                                Append(udf::append100(),
                                    Project([$2:node],
                                        Expand(IN, "Person",
                                            ForeachRelationship(TO, ":hasInterest",
                                                Filter($0.name == %tag,
                                                    NodeScan("Tag")
                                                )
                                            )
                                        )
                                    )
                                ),
                                GroupBy([$4:node],
                                        [count($0:node)],
                                    Expand(OUT, "Person",
                                        ForeachRelationship(FROM, ":hasCreator",
                                            Filter($2.creationDate > %date,
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
                    ),
                    GroupBy([$0:node],
                            [sum($1:uint64)],
                        Union(
                            Append(udf::append100(),
                                Project([$2:node],
                                    Expand(IN, "Person",
                                        ForeachRelationship(TO, ":hasInterest",
                                            Filter($0.name == %tag,
                                                NodeScan("Tag")
                                            )
                                        )
                                    )
                                )
                            ),
                            GroupBy([$4:node],
                                    [count($0:node)],
                                Expand(OUT, "Person",
                                    ForeachRelationship(FROM, ":hasCreator",
                                        Filter($2.creationDate > %date,
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