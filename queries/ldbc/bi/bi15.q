Project([$6, $5],
    Sort([$5:double DESC, udf::sortedId(tuple) ASC],
        AppendToTuple(udf::appendWeight(tuple),
            AppendToTuple(udf::appendIds(tuple),
                AlgoShortestPath(UNWEIGHTED, {0, 2}, udf::rshipPred, TRUE, TRUE,
                    Limit(1,
                        Filter($2.id == %id2,
                            Expand(OUT, "Person",
                                ForeachRelationship(FROM, ":knows", {1, 100},
                                    Filter($0.id == %id1,
                                        NodeScan("Person")
                                    )
                                )
                            )
                        )
                    )
            )
        )
    )
)