Aggregate([$0:COUNT],
    Group([$3],
        Filter(Lambda5,
            Expand(OUT, "Person",
                ForeachRelationship(FROM, ":hasCreator",
                    Expand(IN, "Comment",
                        ForeachRelationship(TO, ":replyOf", $7,
                            Filter($13.Lambda4,
                                Filter($Lambda3,
                                    Filter($Lambda2,
                                        Expand(IN, "Forum",
                                            ForeachRelationship(TO, ":containerOf", $7,
                                                Filter($10.name == %tag,
                                                    Expand(OUT, "Tag",
                                                        ForeachRelationship(FROM, ":hasTag", $7,
                                                            Filter($Lambda1,
                                                                Project([$0, $2, $2.creationDate:datetime, $4, $6, $8, $15, $17, $17.creationDate:datetime],
                                                                    Expand(IN, "Post",
                                                                        ForeachRelationship(TO, ":hasCreator", $8,
                                                                            Join(HASHJOIN_ON_NODE, {6, 4},
                                                                                Expand(OUT, "Person",
                                                                                    ForeachRelationship(FROM, ":hasMember",
                                                                                        Expand(IN, "Forum",
                                                                                            ForeachRelationship(TO, ":containerOf",
                                                                                                Expand(IN, "Post",
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
                                                                                Expand(OUT, "Person",
                                                                                    ForeachRelationship(FROM, ":hasMember",
                                                                                        Expand(IN, "Forum",
                                                                                            ForeachRelationship(TO, ":containerOf", $2,
                                                                                                Expand(OUT, "Person",
                                                                                                    ForeachRelationship(FROM, ":hasCreator",
                                                                                                        Expand(IN, "Post",
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

Aggregate([$0:COUNT],
    Group([$3],
        Filter(Lambda5,
            Expand(OUT, "Person",
                ForeachRelationship(FROM, ":hasCreator",
                    Expand(IN, "Comment",
                        ForeachRelationship(TO, ":replyOf", $7,
                            Filter($13.Lambda4,
                                Filter($Lambda3,
                                    Filter($Lambda2,
                                        Expand(IN, "Forum",
                                            ForeachRelationship(TO, ":containerOf", $7,
                                                Filter($10.name == %tag,
                                                    Expand(OUT, "Tag",
                                                        ForeachRelationship(FROM, ":hasTag", $7,
                                                            Filter($Lambda1,
                                                                Project([$0, $2, $2.creationDate:datetime, $4, $6, $8, $15, $17, $17.creationDate:datetime],
                                                                    Expand(IN, "Comment",
                                                                        ForeachRelationship(TO, ":hasCreator", $8,
                                                                            Join(HASHJOIN_ON_NODE, {6, 4},
                                                                                Expand(OUT, "Person",
                                                                                    ForeachRelationship(FROM, ":hasMember",
                                                                                        Expand(IN, "Forum",
                                                                                            ForeachRelationship(TO, ":containerOf",
                                                                                                Expand(IN, "Post",
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
                                                                                Expand(OUT, "Person",
                                                                                    ForeachRelationship(FROM, ":hasMember",
                                                                                        Expand(IN, "Forum",
                                                                                            ForeachRelationship(TO, ":containerOf", $2,
                                                                                                Expand(OUT, "Person",
                                                                                                    ForeachRelationship(FROM, ":hasCreator",
                                                                                                        Expand(IN, "Post",
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

Aggregate([$0:COUNT],
    Group([$3],
        Filter(Lambda5,
            Expand(OUT, "Person",
                ForeachRelationship(FROM, ":hasCreator",
                    Expand(IN, "Comment",
                        ForeachRelationship(TO, ":replyOf", $7,
                            Filter($13.Lambda4,
                                Filter($Lambda3,
                                    Filter($Lambda2,
                                        Expand(IN, "Forum",
                                            ForeachRelationship(TO, ":containerOf", $7,
                                                Filter($10.name == %tag,
                                                    Expand(OUT, "Tag",
                                                        ForeachRelationship(FROM, ":hasTag", $7,
                                                            Filter($Lambda1,
                                                                Project([$0, $2, $2.creationDate:datetime, $4, $8, $10, $19, $21, $21.creationDate:datetime],
                                                                    Expand(IN, "Post",
                                                                        ForeachRelationship(TO, ":hasCreator", $10,
                                                                            Join(HASHJOIN_ON_NODE, {8, 6},
                                                                                Expand(OUT, "Person",
                                                                                    ForeachRelationship(FROM, ":hasMember",
                                                                                        Expand(IN, "Forum",
                                                                                            ForeachRelationship(TO, ":containerOf",
                                                                                                Expand(OUT, "Post",
                                                                                                    ForeachRelationship(FROM, [1, 100], ":replyOf",
                                                                                                        Expand(IN, "Comment",
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
                                                                                ),
                                                                                Expand(OUT, "Person",
                                                                                    ForeachRelationship(FROM, ":hasMember",
                                                                                        Expand(IN, "Forum",
                                                                                            ForeachRelationship(TO, ":containerOf", $2,
                                                                                                Expand(OUT, "Post",
                                                                                                    ForeachRelationship(FROM, [1, 100], ":replyOf", $2,
                                                                                                        Expand(OUT, "Person",
                                                                                                            ForeachRelationship(FROM, ":hasCreator",
                                                                                                                Expand(IN, "Comment",
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

Aggregate([$0:COUNT],
    Group([$3],
        Filter(Lambda5,
            Expand(OUT, "Person",
                ForeachRelationship(FROM, ":hasCreator",
                    Expand(IN, "Comment",
                        ForeachRelationship(TO, ":replyOf", $7,
                            Filter($13.Lambda4,
                                Filter($Lambda3,
                                    Filter($Lambda2,
                                        Expand(IN, "Forum",
                                            ForeachRelationship(TO, ":containerOf", $7,
                                                Filter($10.name == %tag,
                                                    Expand(OUT, "Tag",
                                                        ForeachRelationship(FROM, ":hasTag", $7,
                                                            Filter($Lambda1,
                                                                Project([$0, $2, $2.creationDate:datetime, $4, $8, $10, $19, $21, $21.creationDate:datetime],
                                                                    Expand(IN, "Comment",
                                                                        ForeachRelationship(TO, ":hasCreator", $10,
                                                                            Join(HASHJOIN_ON_NODE, {8, 6},
                                                                                Expand(OUT, "Person",
                                                                                    ForeachRelationship(FROM, ":hasMember",
                                                                                        Expand(IN, "Forum",
                                                                                            ForeachRelationship(TO, ":containerOf",
                                                                                                Expand(OUT, "Post",
                                                                                                    ForeachRelationship(FROM, [1, 100], ":replyOf",
                                                                                                        Expand(IN, "Comment",
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
                                                                                ),
                                                                                Expand(OUT, "Person",
                                                                                    ForeachRelationship(FROM, ":hasMember",
                                                                                        Expand(IN, "Forum",
                                                                                            ForeachRelationship(TO, ":containerOf", $2,
                                                                                                Expand(OUT, "Post",
                                                                                                    ForeachRelationship(FROM, [1, 100], ":replyOf", $2,
                                                                                                        Expand(OUT, "Person",
                                                                                                            ForeachRelationship(FROM, ":hasCreator",
                                                                                                                Expand(IN, "Comment",
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