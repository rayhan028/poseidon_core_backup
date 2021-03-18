Project([$6, $5],
    Sort([$5:ASC],
        AppendToTuple($3.Lambda3,
            AppendToTuple($3.Lambda2,
                AlgoShortestPath(UNWEIGHTED, Lambda1, TRUE;
                    Filter($2.id == %id2,
                        Expand(OUT, "Person",
                            ForeachRelationship(FROM, [1, 100] ":knows",
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