Limit(10,
    Sort([$2:datetime DESC, $0:uint64 ASC],
        Union(
            Project([$2.id:uint64, $2.content:string, $2.creationDate:datetime, $4.id:uint64, $6.id:uint64, $6.firstName:string, $6.lastName:string],
                Expand(OUT, 'Person',
                    ForeachRelationship(FROM, ':hasCreator',
                        Expand(OUT, 'Post',
                            ForeachRelationship(FROM, ':replyOf', 1, 100,
                                Expand(IN, 'Comment',
                                    ForeachRelationship(TO, ':hasCreator',
                                        Filter($0.id == 65, 
                                            NodeScan('Person')
                                        )
                                    )
                                )
                            )
                        )
                    )
                )
            ),
            Project([$2.id:uint64, udf::getPost($2:qresult), $2.creationDate:datetime, $2.id:uint64, $0.id:uint64, $0.firstName:string, $0.lastName:string],
                Expand(IN, 'Post',
                    ForeachRelationship(TO, ':hasCreator',
                        Filter($0.id == 65, 
                            NodeScan('Person')
                        )
                    )
                )
            )
        )
    )
)