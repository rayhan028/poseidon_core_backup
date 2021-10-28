Project([$0.id:uint64, $1.id:uint64, $2.name:string, $3:uint64],
    Sort([$3:uint64 DESC, $0.id:uint64 ASC, $1.id:uint64 ASC],
        GroupBy([$0:node, $1:node, $2:node],
                [sum($3:uint64)],
            Union(
                Append(udf::appendScore(1),
                    Project([$1:node, $3:node, $0:node],
                        Filter(udf::filterRship($6:qresult, ":likes"),
                            Filter(udf::rshipExists($3:node, $5:node),
                                Expand(IN, "Post", "Comment",
                                    ForeachRelationship(TO, ":hasCreator", $1,
                                        CrossJoin(
                                            Project([$2:node, $4:node],
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
                                            Project([$2:node, $4:node],
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
                Append(udf::appendScore(10),
                    Project([$1:node, $3:node, $0:node],
                        Filter(udf::filterRship($6:qresult, ":likes"),
                            Filter(udf::rshipExists($1:node, $3:node),
                                Expand(IN, "Post", "Comment",
                                    ForeachRelationship(TO, ":hasCreator", $3,
                                        CrossJoin(
                                            Project([$2:node, $4:node],
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
                                            Project([$2:node, $4:node],
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
                Append(udf::appendScore(15),
                    Project([$1:node, $3:node, $0:node],
                        Filter(udf::knowsRship($4:qresult, $5:qresult),                        
                            Filter(udf::rshipExists($1:node, $3:node),
                                Filter(udf::rshipExists($3:node, $1:node),
                                    CrossJoin(
                                        Project([$2:node, $4:node],
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
                                        Project([$2:node, $4:node],
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
                ),
                Append(udf::appendScore(1),
                    Project([$1:node, $3:node, $0:node],
                        Filter(udf::filterRship($8:qresult, ":replyOf"),
                            Filter(udf::rshipExists($7:node, $5:node),
                                Expand(IN, "Comment",
                                    ForeachRelationship(TO, ":hasCreator", $3,
                                        Expand(IN, "Post", "Comment",
                                            ForeachRelationship(TO, ":hasCreator", $1,
                                                CrossJoin(
                                                    Project([$2:node, $4:node],
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
                                                    Project([$2:node, $4:node],
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
                Append(udf::appendScore(4),
                    Project([$1:node, $3:node, $0:node],
                        Filter(udf::filterRship($8:qresult, ":replyOf"),
                            Filter(udf::rshipExists($5:node, $7:node),
                                Expand(IN, "Post", "Comment",
                                    ForeachRelationship(TO, ":hasCreator", $3,
                                        Expand(IN, "Comment",
                                            ForeachRelationship(TO, ":hasCreator", $1,
                                                CrossJoin(
                                                    Project([$2:node, $4:node],
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
                                                    Project([$2:node, $4:node],
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