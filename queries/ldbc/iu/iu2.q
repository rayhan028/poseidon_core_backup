Create(($0)-[r:likes { creationDate: "2012-06-10T03:00:31.490+0000"} ]->($1),
    CrossJoin(
        Filter($0.id == 933,
            NodeScan("Person")
        ),    
        Filter($0.id == 1976, 
            NodeScan("Post")
        )
    )
)