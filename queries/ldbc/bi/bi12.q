Sort([$1:uint64 DESC, $0:uint64 DESC],
    GroupBy([$3],
            [count($0)],
        AppendToTuple(udf::appendSum(tuple),
            Outerjoin(udf::joinPred(tuple1, tuple2),
                GroupBy([$0],
                        [sum($1)],
                    Union(
                        GroupBy([$0],
                                [count($0)],
                            Filter($6.language:string IN %languages
                                Expand(OUT, "Post",
                                    ForeachRelationship(FROM, ":replyOf", {1, 100}, $1,
                                        Filter(postFeatures(tuple),
                                            Project([$0, $2, udf::hasContent1($2), $2.length:int, $2.creationDate:datetime],
                                                Expand(IN, "Comment",
                                                    ForeachRelationship(TO, ":hasCreator",
                                                        NodeScan("Person")
                                                    )
                                                )
                                            )
                                        )
                                    )
                                )
                            )
                        ),
                        GroupBy([$0],
                                [count($0)],
                            Filter(postFeatures(tuple),
                                Project([$0, udf::hasContent1($2), $2.length:int, $2.creationDate:datetime, udf::hasContent2($2)],
                                    Expand(IN, "Post",
                                        ForeachRelationship(TO, ":hasCreator",
                                            NodeScan("Person")
                                        )
                                    )
                                )
                            )
                        )
                    )
                ),
                NodeScan("Person")
            )
        )
    )
)