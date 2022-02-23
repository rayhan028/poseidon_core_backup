Create(($0)-[r:likes { creationDate: "2012-02-09T08:35:10.880+0000" } ]->($1),
    CrossJoin(
        Filter($0.id == 933,
            NodeScan("Person")
        ),
        Filter($0.id == 1877, 
            NodeScan("Comment")
        )
    )
)