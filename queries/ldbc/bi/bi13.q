Project([$0.id:uint64, $5:uint64, $6:uint64, $7:double],
    Sort([$7:double DESC, $0:uint64 ASC],
        Append(udf::appendScore2($5:uint64, $6:uint64),
            Append(udf::appendScore1($4:qresult),
                Append(udf::appendScore1($2:qresult),
                    LeftOuterJoin($0.id == $0.id,
                        LeftOuterJoin($0.id == $0.id,
                            Project([$0:node],
                                Filter(udf::filterAvgMsgs($3:uint64),
                                    LeftOuterJoin($0.id == $0.id,
                                        Project([$4:node],
                                            Filter($4.creationDate < %date,
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
                                    ),
                                    Append(udf::appendAvgMsgs($0.creationDate:datetime, %dt, $1:uint64),
                                        GroupBy([$4:node],
                                                [count($0:node)],
                                            Filter($6.creationDate < %date,
                                                Expand(IN, "Post", "Comment",
                                                    ForeachRelationship(TO, ":hasCreator",
                                                        Filter($4.creationDate < %date,
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
                            GroupBy([$0:node],
                                    [count($0:node)],
                                HashJoin([$4:node, $0:node],    
                                    Expand(IN, "Person",
                                        ForeachRelationship(TO, ":likes",
                                            Expand(IN, "Post", "Comment",
                                                ForeachRelationship(TO, ":hasCreator",
                                                    Project([$0:node],
                                                        Filter(udf::filterAvgMsgs($3:uint64),
                                                            LeftOuterJoin($0.id == $0.id,
                                                                Project([$4:node],
                                                                    Filter($4.creationDate < %date,
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
                                                            ),
                                                            Append(udf::appendAvgMsgs($0.creationDate:datetime, %dt, $1:uint64),
                                                                GroupBy([$4:node],
                                                                        [count($0:node)],
                                                                    Filter($6.creationDate < %date,
                                                                        Expand(IN, "Post", "Comment",
                                                                            ForeachRelationship(TO, ":hasCreator",
                                                                                Filter($4.creationDate < %date,
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
                                ),
                                Project([$0:node],
                                    Filter(udf::filterAvgMsgs($3:uint64),
                                        LeftOuterJoin($0.id == $0.id,
                                            Project([$4:node],
                                                Filter($4.creationDate < %date,
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
                                        ),
                                        Append(udf::appendAvgMsgs($0.creationDate:datetime, %dt, $1:uint64),
                                            GroupBy([$4:node],
                                                    [count($0:node)],
                                                Filter($6.creationDate < %date,
                                                    Expand(IN, "Post", "Comment",
                                                        ForeachRelationship(TO, ":hasCreator",
                                                            Filter($4.creationDate < %date,
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
                        GroupBy([$0:node],
                                [count($0:node)],
                            Expand(IN, "Person",
                                ForeachRelationship(TO, ":likes",
                                    Expand(IN, "Post", "Comment",
                                        ForeachRelationship(TO, ":hasCreator",
                                            Project([$0:node],
                                                Filter(udf::filterAvgMsgs($3:qresult),
                                                    LeftOuterJoin($0.id == $0.id,
                                                        Project([$4:node],
                                                            Filter($4.creationDate < %date,
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
                                                    ),
                                                    Append(udf::appendAvgMsgs($0.creationDate:datetime, %dt, $1:uint64),
                                                        GroupBy([$4:node],
                                                                [count($0:node)],
                                                            Filter($6.creationDate < %date,
                                                                Expand(IN, "Post", "Comment",
                                                                    ForeachRelationship(TO, ":hasCreator",
                                                                        Filter($4.creationDate < %date,
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
        )
    )
)