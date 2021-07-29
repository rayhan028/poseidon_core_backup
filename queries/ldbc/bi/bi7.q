Limit(100, 
    Sort([$1:uint64 DESC, $0:string ASC],
        Project([$0.name:string, $1],
            GroupBy([$6],
                    [count($0)],
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