auto incrMsgCnt = [&](auto &v) {
    uint64_t intrst = 100;
    return query_result(intrst);
};

Limit(100, 
    Sort([$4:uint64 DESC, $0:uint64 ASC],
        Project([$0.id:uint64, $0.firstName:string, $0.lastName:string, $1, $2],
            GroupBy([$0],
                    [count($0), sum($3:uint64)],
                AppendToTuple(incrMsgCnt(tuple),
                    GroupBy([$0, $1],
                            [count($0)],
                        Project([$0, $1, $3],
                            Filter($3.creationDate >= %date1 && $3.creationDate <= %date2,
                                Expand(IN, "Comment",
                                    ForeachRelationship(TO, ":replyOf", {1, 100}, $1
                                        Project([$0, $2],
                                            Filter($2.creationDate >= %date1 && $2.creationDate <= %date2,
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