Join(RIGHT, HASHJOIN_ON_NODE,
    Aggregate([$0:COUNT],
        Group([$4],
            Expand(OUT, "Person",
                ForeachRelationship(FROM, ":hasCreator",
                    Filter($2.creationDate > %date,
                        Expand(IN, ["Post", "Comment"],
                            ForeachRelationship(TO, ":hasTag",
                                Filter($0.name == %tag,
                                    NodeScan("Tag")
                                )
                            )
                        )
                    )
                )
            )
        )
    )
)


Project([$0, $3],
    AppendToTuple(Lambda1,
        Join(LEFT, HASHJOIN_ON_NODE, [$2, $0],
            Expand(IN, "Person",
                ForeachRelationship(TO, ":hasInterest",
                    Filter($0.name == %tag,
                        NodeScan("Tag")
                    )
                )
            )
        )
    )
)


Filter(Lambda2,
    Aggregate([$0:COUNT],
        Group([$4],
            Expand(OUT, "Person",
                ForeachRelationship(FROM, ":hasCreator",
                    Filter($2.creationDate > %date,
                        Expand(IN, ["Post", "Comment"],
                            ForeachRelationship(TO, ":hasTag",
                                Filter($0.name == %tag,
                                    NodeScan("Tag")
                                )
                            )
                        )
                    )
                )
            )
        )
    )
)


Limit(100, 
    Project([$0, $1, $2],
        Sort([$3:DESC, $0:ASC],
            Project([$0.id:uint64, $1, $2, $3],
                AppendToTuple(Lambda6,
                    AppendToTuple(Lambda5,
                        Union(                                                      // represent queries
                            Filter(Lambda4,
                                AppendToTuple(Lambda3,
                                    Project([$2],
                                        Expand(IN, "Person",
                                            ForeachRelationship(TO, ":hasInterest",
                                                Filter($0.name == %tag,
                                                    NodeScan("Tag")
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
        )
    )
)