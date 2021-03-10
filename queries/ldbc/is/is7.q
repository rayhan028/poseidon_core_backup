

Project([ $2.id:uint64, $2.content:string, $2.creationDate:datetime, $4.id:uint64, $4.firstName:string, $4.lastName:string ],
    Join(LEFT_OUTER, {4, 2}, 
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
        Expand(OUT, "Person"
            ForeachRelationship(FROM, ":hasCreator",
                Filter($0.id == 42,
                    NodeScan("Post")
                )
            )
        )
    )
)

                         
