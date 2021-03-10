Sort([$2:DESC, $0:ASC, $1:ASC],
    Project([$2.id:uint64, $5.id:uint64, $6],
        AlgoShortestPath(WEIGHTED, {2, 5}, Lambda1, Lambda2, TRUE,
            Join(CROSSJOIN,
                Expand(IN, "Person",
                    ForeachRelationship(TO, ":isLocatedIn",
                        Filter($0.name == %city,
                            NodeScan("Place")
                        )
                    )
                ),
                Expand(IN, "Person",
                    ForeachRelationship(TO, ":isLocatedIn",
                        Filter($0.name == %city,
                            NodeScan("Place")
                        )
                    )
                )
            )
        )
    )
)