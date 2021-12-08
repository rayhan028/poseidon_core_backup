Limit(100,
    Sort([$4:uint64 DESC, $0:uint64 ASC],
        Project([$0.id:uint64, $0.firstName:string, $0.lastName:string, $1:uint64, $2:uint64],
            GroupBy([$0:node], [count($0:uint64), sum($3:uint64)],
                Append(udf::incrMsgCnt($2:uint64),
                    GroupBy([$0:node, $1:node],
                            [count($0:node)],
                        Project([$0:node, $1:node, $3:node],
                            Filter($3.creationDate >= %date1 and $3.creationDate <= %date2,
                                Expand(IN, "Comment",
                                    ForeachRelationship(TO, ":replyOf", 1, 100, $1,
                                        Project([$0:node, $2:node],
                                            Filter($2.creationDate >= %date1 and $2.creationDate <= %date2,
                                                Expand(IN, "Post",
                                                    ForeachRelationship(TO, ":hasCreator",
                                                        NodeScan("Person")
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