Create(($0)-[r:knows { creationDate: "2010-07-21T10:45:29.157+0000" } ]->($1),
    CrossJoin(
        Filter($0.id == 838375,
            NodeScan("Person")
        ),
        Filter($0.id == 833579, 
            NodeScan("Person")
        )
    )
)