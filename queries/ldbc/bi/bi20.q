Limit(20,
    Project([$2.id:uint64, $4:double],
        Sort([$4:double DESC, $2.id:uint64 ASC],
            Algorithm(SPSP, 2, 3, BIDIRECTIONAL, WEIGHTED, udf::rpred(), udf::rweight(),
                CrossJoin(
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