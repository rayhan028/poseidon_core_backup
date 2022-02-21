Create(($0)-[r:hasMember { joinDate: "2012-01-06T11:21:05.645+0000" } ]->($1),
    CrossJoin(
        Filter($0.id == 37,
            NodeScan("Forum")
        ),
        Filter($0.id == 90796, 
            NodeScan("Person")
        )
    )
)