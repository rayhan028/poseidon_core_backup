Limit(20, 
    Project([$0.id:uint64, $1:uint64, $5:uint64],
        HashJoin([$0:node, $0:node],
            Filter(udf::filterCnt($3:qresult, %val),
                LeftOuterJoin($0.id == $0.id,
                    GroupBy([$4:node],
                            [count($0:node)],
                        Expand(OUT, "Person",
                            ForeachRelationship(FROM, ":hasCreator",
                                Filter($2.creationDate == %date,
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
                    ),
                    GroupBy([$0:node],
                            [count($0:node)],
                        Filter($0.name == %tag,
                            Expand(OUT, "Tag",
                                ForeachRelationship(FROM, ":hasTag",
                                    Filter($5.creationDate == %date,
                                        Expand(IN, "Post", "Comment",
                                            ForeachRelationship(TO, ":hasCreator",
                                                ForeachRelationship(ALL, ":knows", $0,
                                                    GroupBy([$4:node],
                                                            [count($0:node)],
                                                        Expand(OUT, "Person",
                                                            ForeachRelationship(FROM, ":hasCreator",
                                                                Filter($2.creationDate == %date,
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
            ),
            Filter(udf::filterCnt($3:qresult, %val),
                LeftOuterJoin($0.id == $0.id,
                    GroupBy([$4:node],
                            [count($0:node)],
                        Expand(OUT, "Person",
                            ForeachRelationship(FROM, ":hasCreator",
                                Filter($2.creationDate == %date,
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
                    ),
                    GroupBy([$0:node],
                            [count($0:node)],
                        Filter($0.name == %tag,
                            Expand(OUT, "Tag",
                                ForeachRelationship(FROM, ":hasTag",
                                    Filter($5.creationDate == %date,
                                        Expand(IN, "Post", "Comment",
                                            ForeachRelationship(TO, ":hasCreator",
                                                ForeachRelationship(ALL, ":knows", $0,
                                                    GroupBy([$4:node],
                                                            [count($0:node)],
                                                        Expand(OUT, "Person",
                                                            ForeachRelationship(FROM, ":hasCreator",
                                                                Filter($2.creationDate == %date,
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
        )
    )
)