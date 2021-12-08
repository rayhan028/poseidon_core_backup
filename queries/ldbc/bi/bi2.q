Limit(100, 
    Sort([$3:uint64 DESC, $0:string ASC],
        Append([udf::countDiff($1:uint64, $2:uint64)],
            Project([$0.name:string, $1:uint64, $3:uint64],
                HashJoin([$0:node, $0:node],
                    GroupBy([$2:node],
                            [count($0:node)],
                        Expand(OUT, "Tag",
                            ForeachRelationship(FROM, ":hasTag",
                                Filter(udf::timePeriod1($0.creationDate:datetime, %dt),
                                    NodeScan("Post", "Comment")
                                )
                            )
                        )
                    ),
                    GroupBy([$2:node],
                            [count($0:node)],
                        Expand(OUT, "Tag",
                            ForeachRelationship(FROM, ":hasTag",
                                Filter(udf::timePeriod2($0.creationDate:datetime, %dt),
                                    NodeScan("Post", "Comment")
                                )
                            )
                        )
                    )
                )
            )
        )
    )
)