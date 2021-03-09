Limit(100, 
    Sort([$1:DESC, $0:ASC],
        Project([$0.id:uint64, $1],
            Aggregate([$2:SUM],
                Group([$0],
                    Aggregate([$0:COUNT],
                        Group([$0, $1],
                            Expand(IN, "Person",
                                ForeachRelationship(TO, ":likes",
                                    Expand(IN, ["Post", "Comment"],
                                        ForeachRelationship(TO, ":hasCreator",
                                            Group([$4, $6],
                                                Expand(IN, "Person",
                                                    ForeachRelationship(TO, ":likes", $2
                                                        Expand(OUT, "Person",
                                                            ForeachRelationship(FROM, ":hasCreator",
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