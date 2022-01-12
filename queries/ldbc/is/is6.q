Project([$2.id:uint64, $2.title:string, $4.id:uint64, $4.firstName:string, $4.lastName:string], 
        Expand(OUT, "Person",
            ForeachRelationship(FROM, ":hasModerator",
                Expand(IN, "Forum",
                    ForeachRelationship(TO, ":containerOf",
                        Filter($0.id == 16492674,
                            NodeScan("Post")
                        )
                    )
                )
            )
        )
    )