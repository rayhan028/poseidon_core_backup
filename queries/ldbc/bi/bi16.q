Limit(20, 
    Sort([$0:DESC, $1:ASC],
        Project([$8, $0.id:uint64],
            AppendToTuple(Lambda
                Join(HASHJOIN_ON_NODE, {0, 0},
                    Aggregate([$0:COUNT],
                        Group([$0, $1, $2],
                            Filter($4 <= %count,
                                Aggregate([$0:COUNT],
                                    Group([$0, $1, $3, $5],
                                        Filter($13.name == %tag,
                                            Expand(OUT, "Tag",
                                                ForeachRelationship(FROM, ":hasTag",
                                                    Filter($11.creationDate == %date,
                                                        Expand(IN, ["Post", "Comment"],
                                                            ForeachRelationship(TO, ":hasCreator",
                                                                Expand(OUT, "Person",
                                                                    ForeachRelationship(FROM, ":knows", $3
                                                                        Filter($7.name == %tag,
                                                                            Expand(OUT, "Tag",
                                                                                ForeachRelationship(FROM, ":hasTag",
                                                                                    Filter($5.creationDate == %date,
                                                                                        Expand(IN, ["Post", "Comment"],
                                                                                            ForeachRelationship(TO, ":hasCreator",
                                                                                                Expand(OUT, "Person",
                                                                                                    ForeachRelationship(FROM, ":knows", $0
                                                                                                        Aggregate([$0:COUNT],
                                                                                                            Group([$1],
                                                                                                                Filter($2 <= %count,
                                                                                                                    Aggregate([$0:COUNT],
                                                                                                                        Group([$2, $4],
                                                                                                                            Filter($10.name == %tag,
                                                                                                                                Expand(OUT, "Tag",
                                                                                                                                    ForeachRelationship(FROM, ":hasTag",
                                                                                                                                        Filter($8.creationDate == %date,
                                                                                                                                            Expand(IN, ["Post", "Comment"],
                                                                                                                                                ForeachRelationship(TO, ":hasCreator",
                                                                                                                                                    Expand(OUT, "Person",
                                                                                                                                                        ForeachRelationship(FROM, ":knows",
                                                                                                                                                            Expand(OUT, "Person",
                                                                                                                                                                ForeachRelationship(FROM, ":hasCreator",
                                                                                                                                                                    Filter($2.creationDate == %date,
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
                    ),
                    Aggregate([$0:COUNT],
                        Group([$0, $1, $2],
                            Filter($4 <= %count,
                                Aggregate([$0:COUNT],
                                    Group([$0, $1, $3, $5],
                                        Filter($13.name == %tag,
                                            Expand(OUT, "Tag",
                                                ForeachRelationship(FROM, ":hasTag",
                                                    Filter($11.creationDate == %date,
                                                        Expand(IN, ["Post", "Comment"],
                                                            ForeachRelationship(TO, ":hasCreator",
                                                                Expand(OUT, "Person",
                                                                    ForeachRelationship(FROM, ":knows", $3
                                                                        Filter($7.name == %tag,
                                                                            Expand(OUT, "Tag",
                                                                                ForeachRelationship(FROM, ":hasTag",
                                                                                    Filter($5.creationDate == %date,
                                                                                        Expand(IN, ["Post", "Comment"],
                                                                                            ForeachRelationship(TO, ":hasCreator",
                                                                                                Expand(OUT, "Person",
                                                                                                    ForeachRelationship(FROM, ":knows", $0
                                                                                                        Aggregate([$0:COUNT],
                                                                                                            Group([$1],
                                                                                                                Filter($2 <= %count,
                                                                                                                    Aggregate([$0:COUNT],
                                                                                                                        Group([$2, $4],
                                                                                                                            Filter($10.name == %tag,
                                                                                                                                Expand(OUT, "Tag",
                                                                                                                                    ForeachRelationship(FROM, ":hasTag",
                                                                                                                                        Filter($8.creationDate == %date,
                                                                                                                                            Expand(IN, ["Post", "Comment"],
                                                                                                                                                ForeachRelationship(TO, ":hasCreator",
                                                                                                                                                    Expand(OUT, "Person",
                                                                                                                                                        ForeachRelationship(FROM, ":knows",
                                                                                                                                                            Expand(OUT, "Person",
                                                                                                                                                                ForeachRelationship(FROM, ":hasCreator",
                                                                                                                                                                    Filter($2.creationDate == %date,
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