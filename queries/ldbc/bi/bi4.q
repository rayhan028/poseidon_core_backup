Join(RIGHT, HASHJOIN_ON_NODE,
    Limit(100,
        Sort([$2:DESC, $1:ASC],
            Aggregate([$0:COUNT],
                Group([$0, $1],
                    Project([$6, $6.id:uint64],
                        Filter($6.creationDate > %date,
                            Expand(IN, ["Forum"],
                                ForeachRelationship(TO, ":hasMember",
                                    Expand(IN, ["Person"],
                                        ForeachRelationship(TO, ":isLocatedIn",
                                            Expand(IN, ["Place"],
                                                ForeachRelationship(TO, ":isPartOf",
                                                    Filter($0.name == %country,
                                                        NodeScan(["Place"])
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

Sort([$4:DESC, $0:ASC],
    Project([$0.id:uint64, $0.firstName:string, $0.lastName:string, $0.creationDate:datetime, $1],
        Aggregate([$0:COUNT],
            Group([$0],
                Join(LEFT, HASHJOIN_ON_NODE, [$2, $0],
                    Project([$4, $6, $8],
                        Filter($6.creationDate > %date,
                            Expand(IN, "Forum",
                                ForeachRelationship(TO, ":containerOf",
                                    Expand(IN, ["Post", "Comment"],
                                        ForeachRelationship(TO, ":hasCreator",
                                            Expand(OUT, "Person",
                                                ForeachRelationship(FROM, ":hasMember", $0,
                                                    Limit(100,
                                                        Sort([$2:DESC, $1:ASC],
                                                            Aggregate([$0:COUNT],
                                                                Group([$0, $1],
                                                                    Project([$6, $6.id:uint64],
                                                                        Filter($6.creationDate > %date,
                                                                            Expand(IN, "Forum",
                                                                                ForeachRelationship(TO, ":hasMember",
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