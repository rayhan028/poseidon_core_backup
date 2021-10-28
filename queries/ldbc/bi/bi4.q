Limit(100,
    Sort([$4:uint64 DESC, $0:uint64 ASC],
        Project([$0.id:uint64, $0.firstName:string, $0.lastName:string, $0.creationDate:datetime, $1:uint64],
            GroupBy([$0:node],
                    [count($0:node)],
                Union(
                    HashJoin([$1:node, $0:node],
                        Project([$4:node, $10:node],
                            Filter($6.creationDate > %date,
                                Expand(IN, "Forum",
                                    ForeachRelationship(TO, ":containerOf",
                                        Expand(OUT, "Post",
                                            ForeachRelationship(FROM, ":replyOf", 1, 100,
                                                Expand(IN, "Comment",
                                                    ForeachRelationship(TO, ":hasCreator",
                                                        Expand(OUT, "Person",
                                                            ForeachRelationship(FROM, ":hasMember", $0,
                                                                Limit(100,
                                                                    Sort([$2:uint64 DESC, $1:uint64 ASC],
                                                                        GroupBy([$0:node, $1:uint64],
                                                                                [count($0:node)],
                                                                            Project([$6:node, $6.id:uint64],
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
                        ),
                        Limit(100,
                            Sort([$2:uint64 DESC, $1:uint64 ASC],
                                GroupBy([$0:node, $1:uint64],
                                        [count($0:node)],
                                    Project([$6:node, $6.id:uint64],
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
                    ),
                    HashJoin([$1:node, $0:node],
                        Project([$4:node, $8:node],
                            Filter($6.creationDate > %date,
                                Expand(IN, "Forum",
                                    ForeachRelationship(TO, ":containerOf",
                                        Expand(IN, "Post",
                                            ForeachRelationship(TO, ":hasCreator",
                                                Expand(OUT, "Person",
                                                    ForeachRelationship(FROM, ":hasMember", $0,
                                                        Limit(100,
                                                            Sort([$2:uint64 DESC, $1:uint64 ASC],
                                                                GroupBy([$0:node, $1:uint64],
                                                                        [count($0:node)],
                                                                    Project([$6:node, $6.id:uint64],
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
                        ),
                        Limit(100,
                            Sort([$2:uint64 DESC, $1:uint64 ASC],
                                GroupBy([$0:node, $1:uint64],
                                        [count($0:node)],
                                    Project([$6:node, $6.id:uint64],
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