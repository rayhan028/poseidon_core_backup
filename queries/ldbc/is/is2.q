Project([$2.id:uint64, $2.creationDate:datetime, $2.id:uint64, $0.id:uint64, $0.firstName:string, $0.lastName:string],
    Limit(10, 
        Expand(IN, "Post",
            ForeachRelationship(TO, ":hasCreator",
                Filter($0.id == 42, 
                    NodeScan("Person")
                )
            )
        )
    )
)