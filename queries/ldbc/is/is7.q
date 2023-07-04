Project([ $1.id:uint64, $1.content:string, $1.creationDate:datetime, $2.id:uint64, $2.firstName:string, $2.lastName:string, udf::replyAuthorKnowsOriginalMessageAuthor($3:qresult) ],
    Sort([$4:datetime ASC, $5:uint64 DESC],
        Project([$2:qresult, $4:qresult, $7:qresult, $2.creationDate:datetime, $2.id:uint64],
            LeftOuterJoin(udf::nodesConnected($4:qresult, $7:qresult),
                Expand(OUT, 'Person',
                    ForeachRelationship(FROM, ':hasCreator',
                        Filter($0.id == 16492676,
                            NodeScan(['Post', 'Comment'])
                        )
                    )
                ),
                Expand(OUT, 'Person', 
                    ForeachRelationship(FROM, ':hasCreator',
                        Expand(IN, 'Comment', 
                            ForeachRelationship(TO, ':replyOf',
                                Filter($0.id == 16492676,
                                    NodeScan(['Post', 'Comment'])
                                )
                            )
                        )
                    )
                )
            )   
        ) 
    )
)