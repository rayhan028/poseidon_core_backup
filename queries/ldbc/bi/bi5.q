Limit(100, 
    Sort([$4:DESC, $0:ASC],
        Project([$0.id:uint64, $1, $3, $5, $6],
            Filter(Lambda,
                Join(NESTEDLOOPJOIN_ON_NODE, {0, 0},
                    Join(NESTEDLOOPJOIN_ON_NODE, {0, 0},
                        Aggregate([$0:COUNT],
                            Group([$4],
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
                        ),
                        Aggregate([$0:COUNT],
                            Group([$4],
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
                    ),
                    Aggregate([$0:COUNT],
                        Group([$4],
                            Expand(IN, "Comment",
                                ForeachRelationship(TO, ":replyOf", $2
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