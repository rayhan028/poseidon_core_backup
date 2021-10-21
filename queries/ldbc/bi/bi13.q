Limit(100,
    Sort([$3:double DESC, $0:uint64 ASC],
        Project([$0.id:uint64, $5, $6, $7],
            AppendToTuple(udf::appendScore2(tuple),
                AppendToTuple(udf::appendScore1(tuple, 4),
                    AppendToTuple(udf::appendScore1(tuple, 2),
                        Outerjoin(udf::joinPred(tuple1, tuple2),
                            Outerjoin(udf::joinPred(tuple1, tuple2),
                                Project([$0],
                                    Filter(udf::zmbMsgCnt(tuple),
                                        Outerjoin(udf::joinPred(tuple1, tuple2),
                                            Project([$4],
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
                                            ),
                                            AppendToTuple(udf::avgMsgs(tuple),
                                                Project([$0, $1, $0.creationDate:datetime],
                                                    GroupBy([$4],
                                                            [count($0)],
                                                        Filter($6.creationDate < %date,
                                                            Expand(IN, ["Post", "Comment"],
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
                                Join(HASHJOIN_ON_NODE, {$4, $0},
                                    Expand(IN, "Person",
                                        ForeachRelationship(TO, ":likes",
                                            Expand(IN, ["Post", "Comment"],
                                                ForeachRelationship(TO, ":hasCreator",
                                                    Project([$0],
                                                        Filter(udf::zmbMsgCnt(tuple),
                                                            Outerjoin(udf::joinPred(tuple1, tuple2),
                                                                Project([$4],
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
                                                                ),
                                                                AppendToTuple(udf::avgMsgs(tuple),
                                                                    Project([$0, $1, $0.creationDate:datetime],
                                                                        GroupBy([$4],
                                                                                [count($0)],
                                                                            Filter($6.creationDate < %date,
                                                                                Expand(IN, ["Post", "Comment"],
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
                                    ),
                                    Project([$0],
                                        Filter(udf::zmbMsgCnt(tuple),
                                            Outerjoin(udf::joinPred(tuple1, tuple2),
                                                Project([$4],
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
                                                ),
                                                AppendToTuple(udf::avgMsgs(tuple),
                                                    Project([$0, $1, $0.creationDate:datetime],
                                                        GroupBy([$4],
                                                                [count($0)],
                                                            Filter($6.creationDate < %date,
                                                                Expand(IN, ["Post", "Comment"],
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
                            ),
                            GroupBy([$0],
                                    [count($0)],
                                Expand(IN, "Person",
                                    ForeachRelationship(TO, ":likes",
                                        Expand(IN, ["Post", "Comment"],
                                            ForeachRelationship(TO, ":hasCreator",
                                                Project([$0],
                                                    Filter(udf::zmbMsgCnt(tuple),
                                                        Outerjoin(udf::joinPred(tuple1, tuple2),
                                                            Project([$4],
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
                                                            ),
                                                            AppendToTuple(udf::avgMsgs(tuple),
                                                                Project([$0, $1, $0.creationDate:datetime],
                                                                    GroupBy([$4],
                                                                            [count($0)],
                                                                        Filter($6.creationDate < %date,
                                                                            Expand(IN, ["Post", "Comment"],
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
        )
    )
)