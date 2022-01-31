Project([ $2.id:uint64, $2.content:string, $2.creationDate:datetime, $4.id:uint64, $4.firstName:string, $4.lastName:string, udf::replyAuthorKnowsOriginalMessageAuthor($7:qresult) ],
    Sort([$2.creationDate:datetime ASC, $2.id:uint64 DESC],
            LeftOuterJoin(udf::nodesConnected($4:node, $2:node),
                Expand(OUT, "Person", 
                    ForeachRelationship(FROM, ":hasCreator",
                        Expand(IN, "Comment", 
                            ForeachRelationship(TO, ":replyOf",
                                Filter($0.id == 16492676,
                                    NodeScan("Post", "Comment")
                                )
                            )
                        )
                    )
                ),
                Expand(OUT, "Person",
                    ForeachRelationship(FROM, ":hasCreator",
                        Filter($0.id == 16492676,
                            NodeScan("Post", "Comment")
                        )
                    )
                )
        )
    )
)