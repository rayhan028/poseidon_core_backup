Sort([$1:uint64 DESC, $0:uint64 DESC],
    GroupBy([$3:uint64],
            [count($0:node)],
        Append(udf::appendSum($1:qresult, $2:uint64),
            LeftOuterJoin($0.id == $0.id,
                NodeScan("Person"),
                GroupBy([$0:node],
                        [sum($1:uint64)],
                    Union(
                        GroupBy([$0:node],
                                [count($0:node)],
                            Filter($4.language in %languages,
                                Expand(OUT, "Post",
                                    ForeachRelationship(FROM, ":replyOf", 1, 100, $2,
                                        Filter(udf::hasContent($2:qresult) and $2.length < %length 
                                                and $2.creationDate > %time,
                                            Expand(IN, "Comment",
                                                ForeachRelationship(TO, ":hasCreator",
                                                    NodeScan("Person")
                                                )
                                            )
                                        )
                                    )
                                )
                            )
                        ),
                        GroupBy([$0:node],
                                [count($0:uint64)],
                            Filter(udf::hasContent($2:qresult) and $2.length < %length 
                                    and $2.creationDate > %time and udf::language($2:qresult) in %languages,
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