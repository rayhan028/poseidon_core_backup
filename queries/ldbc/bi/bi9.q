Limit(100, 
    Sort([$4:DESC, $0:ASC],
        Project([$0.id:uint64, $0.firstName:string, $0.lastName:string, $1, $2],
            Aggregate([$0:COUNT, $3:SUM],
                Group([$0],
                    AppendToTuple(Lambda,
                        Aggregate([$0:COUNT],
                            Group([$0, $1],
                                Project([$0, $1, $3],
                                    Filter($3.creationDate >= %dt1 && $3.creationDate <= %dt2,
                                        Expand(IN, "Comment",
                                            ForeachRelationship(TO, [1, 100], ":replyOf", $1
                                                Project([$0, $2],
                                                    Filter($2.creationDate >= %dt1 && $2.creationDate <= %dt2,
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
    )
)