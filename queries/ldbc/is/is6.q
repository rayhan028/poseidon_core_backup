Union(
    Project([$4.id:uint64, $4.title:string, $6.id:uint64, $6.firstName:string, $6.lastName:string], 
        Expand(OUT, "Person",
            ForeachRelationship(FROM, ":hasModerator",
                Expand(IN, "Forum",
                    ForeachRelationship(TO, ":containerOf",
                        Expand(OUT, "Post",
                            ForeachRelationship(FROM, ":replyOf", 1, 100,
                                Filter($0.id == 42,
                                    NodeScan("Comment")
                                )
                            )
                        )
                    )
                )
            )
        )
    ),
    Project([$2.id:uint64, $2.title:string, $4.id:uint64, $4.firstName:string, $4.lastName:string], 
        Expand(OUT, "Person",
            ForeachRelationship(FROM, ":hasModerator",
                Expand(IN, "Forum",
                    ForeachRelationship(TO, ":containerOf",
                        Filter($0.id == 42,
                            NodeScan("Post")
                        )
                    )
                )
            )
        )
    )
)