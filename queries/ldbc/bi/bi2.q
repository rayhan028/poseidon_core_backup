Limit(100, 
    Sort([$3:DESC, $0:ASC],
        AppendToTuple(Lambda2
            Project([$0.name:string, $1, $3],
                Join(HASHJOIN_ON_NODE, {0, 0},
                    Aggregate([$0:COUNT],
                        Group([$2],
                            Expand(OUT, "Tag",
                                ForeachRelationship(FROM, ":hasTag",
                                    Filter($0.creationDate:Lambda1,
                                        NodeScan(["Post", "Comment"])
                                    )
                                )
                            )
                        )
                    ),
                    Aggregate([$0:COUNT],
                        Group([$2],
                            Expand(OUT, "Tag",
                                ForeachRelationship(FROM, ":hasTag",
                                    Filter($0.creationDate:Lambda1,
                                        NodeScan(["Post", "Comment"])
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