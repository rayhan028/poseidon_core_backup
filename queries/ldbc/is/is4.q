Project([$0.creationDate:datetime, udf::getMessage($0:qresult)],
    Filter($0.id == 1374389595,
        NodeScan("Post")
    )
)