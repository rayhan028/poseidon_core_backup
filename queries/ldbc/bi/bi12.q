Sort([$1:DESC, $0:DESC],
    Aggregate([$0:COUNT],
        Group([$4],
            AppendToTuple(Lambda3,
                Join(HASHJOIN_ON_NODE, {0, 0},
                    Aggregate([$0:COUNT],
                        Group([$0],
                            Filter($2.Lambda1,
                                Expand(IN, "Post",
                                    ForeachRelationship(TO, ":hasCreator",
                                        NodeScan("Person")
                                    )
                                )
                            )
                        )
                    ),
                    Aggregate([$0:COUNT],
                        Group([$0],
                            Filter($4.Lambda2,
                                Expand(OUT, "Post",
                                    ForeachRelationship(FROM, [1, 100] ":replyOf", $2,
                                        Filter($2.Lambda1,
                                            Expand(IN, "Post",
                                                ForeachRelationship(TO, ":hasCreator",
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
            )
        )
    )
)