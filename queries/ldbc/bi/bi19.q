Limit(20,
    Project([$2.id:uint64, $5.id:uint64, $6:double],
        Sort([$6:double DESC, $2.id:uint64 ASC, $5.id:uint64 ASC],
            Algorithm(SPSP, 2, 5, BIDIRECTIONAL, WEIGHTED, udf::rPred(), udf::rweight(),
                CrossJoin(
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