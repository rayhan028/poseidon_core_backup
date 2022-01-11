Sort([$3:datetime DESC, $0:uint64 ASC],
    Project([$2.id:uint64, $2.firstName:string, $2.lastName:string, $1.creationDate:datetime], 
        Expand(OUT, "Person",
            ForeachRelationship(FROM, ":knows",
                Filter($0.id == 933, 
                    NodeScan("Person")
                )
            )
        )
    )
)