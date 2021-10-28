Project([$4:array, $5:double],
    Sort([$5:double DESC, udf::sortedId($4:array) ASC],
        Append(udf::appendWeight($3:array),
            Append(udf::appendIds($3:array),
                Algorithm(SPSP, 0, 2, BIDIRECTIONAL, UNWEIGHTED, udf::rshipPred(), ALLPATHS,
                    Limit(1,
                        Filter($2.id == %id2,
                            Expand(OUT, "Person",
                                ForeachRelationship(FROM, ":knows", 1, 100,
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
)