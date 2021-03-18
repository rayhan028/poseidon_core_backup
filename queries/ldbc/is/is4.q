Project([$0.creationDate:datetime],
    Filter($0.id == 22,
        NodeScan("Post")
    )
)