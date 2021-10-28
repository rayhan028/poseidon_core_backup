Limit(20,
    Sort([$4:uint64 DESC, $0:uint64 ASC],
        Project([$1.id:uint64, $1.title:string, $1.creationDate:datetime, $0.id:uint64, $2:uint64],
            GroupBy([$0.id:int, $1.id:int], 
                    [count($0:node)],
                Union(
                    GroupBy([$4:node, $6:node, $8:node],
                        Filter($12.name == %tagClass,
                            Expand(OUT, "TagClass",
                                ForeachRelationship(FROM, ":hasType",
                                    Expand(OUT, "Tag",
                                        ForeachRelationship(FROM, ":hasTag",
                                            Expand(IN, "Comment",
                                                ForeachRelationship(TO, ":replyOf", 1, 100,
                                                    Expand(OUT, "Post",
                                                        ForeachRelationship(FROM, ":containerOf",
                                                            Expand(IN, "Forum",
                                                                ForeachRelationship(TO, ":hasModerator",
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
                    ),
                    GroupBy([$4:node, $6:node, $8:node],
                        Filter($12.name == %tagClass,
                            Expand(OUT, "TagClass",
                                ForeachRelationship(FROM, ":hasType",
                                    Expand(OUT, "Tag",
                                        ForeachRelationship(FROM, ":hasTag",
                                            Expand(OUT, "Post",
                                                ForeachRelationship(FROM, ":containerOf",
                                                    Expand(IN, "Forum",
                                                        ForeachRelationship(TO, ":hasModerator",
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