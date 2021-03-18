Project([$1],
    Aggregate([$0:COUNT],
        Group([$0],
            Filter(Lambda,
                Project([$4 $5, $6],
                    Filter($8.creationDate > %date,
                        Join(JOIN_ON_RSHIP, {4, 0},
                            Project([$4 $9],
                                Filter($10.creationDate > %date,
                                    Join(JOIN_ON_RSHIP, {4, 4},
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
                                        ),
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