Sort([$3:DESC, $0:DESC, $1:ASC],
    Project([$1.id:uint64, $3.id:uint64, $0.name:string, $11],
        AppendToTuple(Lambda1,
            AppendToTuple(Lambda2,
                AppendToTuple(Lambda3,
                    AppendToTuple(Lambda4,
                        AppendToTuple(Lambda5,
                            AppendToTuple(Lambda6,
                                AppendToTuple(Lambda7,
                                    AppendToTuple(Lambda8,
                                        Join(CROSSJOIN,
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