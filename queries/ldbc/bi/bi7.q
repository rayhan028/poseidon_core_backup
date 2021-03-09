Limit(100, 
    Sort([$1:DESC, $0:ASC],
        Project([$0.name:string, $1],
            Aggregate([$0:COUNT],
                Group([$6],
                    Filter($6.name != %tag,
                        Expand(OUT, "Tag",
                            ForeachRelationship(FROM, ":hasTag",
                                Expand(IN, "Comment",
                                    ForeachRelationship(TO, ":replyOf",
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