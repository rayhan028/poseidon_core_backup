Project([ $2.id:uint64, $2.content:string, $2.creationDate:datetime, $4.id:uint64, $4.firstName:string, $4.lastName:string ],
    LeftOuterJoin($4.id == $2.id, 
        Expand(OUT, "Person", 
            ForeachRelationship(FROM, ":hasCreator",
                Expand(IN, "Comment", 
                    ForeachRelationship(TO, ":replyOf",
                        Filter($0.id == 42,
                            NodeScan("Post")
                        )
                    )
                )
            )
        ),
        Expand(OUT, "Person",
            ForeachRelationship(FROM, ":hasCreator",
                Filter($0.id == 42,
                    NodeScan("Post")
                )
            )
        )
    )
)