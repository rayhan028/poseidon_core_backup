Limit(100,
    Sort([$4:uint64 DESC, $0:uint64 ASC],
        Project([$0.id:uint64, $0.firstName:string, $0.lastName:string, $0.creationDate:datetime, $1:node],
            GroupBy([$0],
                    [count($0)],
                Union(
                    Hashjoin({$1, $0},
                        Project([$4, $10],
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
                                                                        GroupBy([$0, $1],
                                                                                [count($0)],
                                                                            Project([$6, $6.id:uint64],
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
                                GroupBy([$0, $1],
                                        [count($0)],
                                    Project([$6, $6.id:uint64],
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
                    Hashjoin({$1, $0},
                        Project([$4, $8],
                            Filter($6.creationDate > %date,
                                Expand(IN, "Forum",
                                    ForeachRelationship(TO, ":containerOf",
                                        Expand(IN, ["Post"],
                                            ForeachRelationship(TO, ":hasCreator",
                                                Expand(OUT, "Person",
                                                    ForeachRelationship(FROM, ":hasMember", $0,
                                                        Limit(100,
                                                            Sort([$2:uint64 DESC, $1:uint64 ASC],
                                                                GroupBy([$0, $1],
                                                                        [count($0)],
                                                                    Project([$6, $6.id:uint64],
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
                                GroupBy([$0, $1],
                                        [count($0)],
                                    Project([$6, $6.id:uint64],
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