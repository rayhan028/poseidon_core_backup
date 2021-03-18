Sort([$0:ASC],
    Project([$2.id:uint64, $4],
        AlgoShortestPath(TOPKWEIGHTED, {2, 3}, 20, Lambda1, Lambda2, TRUE,
            Join(CROSSJOIN,
                Filter($0.id == %pid,
                    NodeScan("Person")
                ),
                Expand(IN, "Person",
                    ForeachRelationship(TO, ":workAt",
                        Filter($0.name == %orhanisation,
                            NodeScan("Organisation")
                        )
                    )
                )
            )
        )
    )
)