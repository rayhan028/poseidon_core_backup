Limit(100, 
    Sort([$3:uint64 DESC, $0:string ASC],
        Append([udf::countDiff($0:int)],
            Project([$0.name:string, $1:node, $3:node],
                HashJoin([$0.id:int, $0.id:int],
                    GroupBy([$2.id:int],
                            [count($0.id:int)],
                        Expand(OUT, "Tag",
                            ForeachRelationship(FROM, ":hasTag",
                                Filter(udf::timePeriod1($0.creationDate:datetime),
                                    NodeScan("Post", "Comment")
                                )
                            )
                        )
                    ),
                    GroupBy([$2.id:int],
                            [count($0.id:int)],
                        Expand(OUT, "Tag",
                            ForeachRelationship(FROM, ":hasTag",
                                Filter(udf::timePeriod2($0.creationDate:datetime),
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