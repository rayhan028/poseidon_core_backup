Sort([$3:uint64 DESC, $0:uint64 ASC, $1:uint64 ASC],
    Project([$0.id:uint64, $1.id:uint64, $2.name:string, $3],
        GroupBy([$0, $1, $2],
                [sum($3)],
            Union(
                AppendToTuple(udf::appendScore(tuple, 1),
                    Project([$1, $3, $0]
                        Filter(udf::fltrRship(tuple, 6, ":likes"),
                            RshipExists({3, 5},
                                Expand(IN, ["Post", "Comment"],
                                    ForeachRelationship(TO, ":hasCreator", $1,
                                        Crossjoin(
                                            Project([$2, $4],
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
                                            ),
                                            Project([$2, $4],
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
                AppendToTuple(udf::appendScore(tuple, 10),
                    Project([$1, $3, $0]
                        Filter(udf::fltrRship(tuple, 6, ":likes"),
                            RshipExists({1, 5},
                                Expand(IN, ["Post", "Comment"],
                                    ForeachRelationship(TO, ":hasCreator", $3,
                                        Crossjoin(
                                            Project([$2, $4],
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
                                            ),
                                            Project([$2, $4],
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
                AppendToTuple(udf::appendScore(tuple, 15),
                    Filter(udf::knowsRship(tuple),
                        RshipExists({1, 3},
                            RshipExists({3, 1},
                                Crossjoin(
                                    Project([$2, $4],
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
                                    ),
                                    Project([$2, $4],
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
                ),
                AppendToTuple(udf::appendScore(tuple, 1),
                    Project([$1, $3, $0]
                        Filter(udf::fltrRship(tuple, 8, ":replyOf"),
                            RshipExists({7, 5},
                                Expand(IN, ["Post", "Comment"],
                                    ForeachRelationship(TO, ":hasCreator", $3,
                                        Expand(IN, "Comment",
                                            ForeachRelationship(TO, ":hasCreator", $1,
                                                Crossjoin(
                                                    Project([$2, $4],
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
                                                    ),
                                                    Project([$2, $4],
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
                ),
                AppendToTuple(udf::appendScore(tuple, 4),
                    Project([$1, $3, $0]
                        Filter(udf::connectedNodes(tuple),
                            RshipExists({5, 7},
                                Expand(IN, ["Post", "Comment"],
                                    ForeachRelationship(TO, ":hasCreator", $3,
                                        Expand(IN, "Comment",
                                            ForeachRelationship(TO, ":hasCreator", $1,
                                                Crossjoin(
                                                    Project([$2, $4],
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
                                                    ),
                                                    Project([$2, $4],
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