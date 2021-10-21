Limit(20,
    Sort([$2:double DESC, $0:uint64 ASC, $1:uint64 ASC],
        Project([$2.id:uint64, $5.id:uint64, $6],
            AlgoShortestPath(WEIGHTED, {2, 5}, udf::rPred, udf::rweight, TRUE,
                Crossjoin(
                    Expand(IN, "Person",
                        ForeachRelationship(TO, ":isLocatedIn",
                            Filter($0.id == %id2,
                                NodeScan("Place")
                            )
                        )
                    ),
                    Expand(IN, "Person",
                        ForeachRelationship(TO, ":isLocatedIn",
                            Filter($0.id == %id2,
                                NodeScan("Place")
                            )
                        )
                    )
                )
            )
        )
    )
)