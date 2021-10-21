Limit(20,
    Sort([$1:double DESC, $0:uint64 ASC],
        Project([$2.id:uint64, $4],
            AlgoShortestPath(TOPKWEIGHTED, {2, 3}, udf::rpred, udf::rweight, TRUE,
                Crossjoin(
                    Expand(IN, "Person",
                        ForeachRelationship(TO, ":workAt",
                            Filter($0.name == %organisation,
                                NodeScan("Organisation")
                            )
                        )
                    ),
                    Filter($0.id == %id,
                        NodeScan("Person")
                    )
                )
            )
        )
    )
)