Limit(100, 
    Sort([$3:DESC, $0:ASC],
        Project([$0.id:uint64, $1, $2, $3],
            Filter($2.Lambda2,
                Aggregate([$0:COUNT],
                    Group([$0, $1],
                        Expand(IN, "Person",
                            ForeachRelationship(TO, ":likes",
                                Expand(IN, ["Post", "Comment"],
                                    ForeachRelationship(TO, ":hasCreator", $0
                                        Aggregate([$0:COUNT],
                                            Group([$0],
                                                Join(HASHJOIN_ON_NODE, {5, 0},
                                                    Filter($0.Lambda,
                                                        Aggregate([$0:COUNT],
                                                            Group([$4],
                                                                Expand(IN, ["Post", "Comment"],
                                                                    ForeachRelationship(TO, ":hasCreator",
                                                                        Filter($4.creationDate < %date,
                                                                            Expand(IN, "Person",
                                                                                ForeachRelationship(TO, ":isLocatedIn",
                                                                                    Expand(IN, "Place",
                                                                                        ForeachRelationship(TO, ":isPartOf",
                                                                                            Filter($0.name == %country,
                                                                                                NodeScan("Place")
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
                                                    ),
                                                    Expand(IN, "Person",
                                                        ForeachRelationship(TO, ":likes",
                                                            Expand(IN, ["Post", "Comment"],
                                                                ForeachRelationship(TO, ":hasCreator", $0
                                                                    Filter($0.Lambda1,
                                                                        Aggregate([$0:COUNT],
                                                                            Group([$4],
                                                                                Expand(IN, ["Post", "Comment"],
                                                                                    ForeachRelationship(TO, ":hasCreator",
                                                                                        Filter($4.creationDate < %date,
                                                                                            Expand(IN, "Person",
                                                                                                ForeachRelationship(TO, ":isLocatedIn",
                                                                                                    Expand(IN, "Place",
                                                                                                        ForeachRelationship(TO, ":isPartOf",
                                                                                                            Filter($0.name == %country,
                                                                                                                NodeScan("Place")
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
    )
)