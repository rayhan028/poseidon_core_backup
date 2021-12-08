Limit(20,
    Project([$0.id:uint64, $1:uint64],
        Sort([$1:uint64 DESC, $0.id:uint64 ASC],
            GroupBy([$4:node],
                    [count($0:node)],
                Filter($8.name == %tag,
                    Expand(OUT, "Tag",
                        ForeachRelationship(FROM, ":hasInterest", $4,
                            Filter(udf::knowsRship($0:node, $4:node, $5:qresult, $6:qresult),
                                Filter(udf::rshipExists($4:node, $0:node),
                                    Filter(udf::rshipExists($0:node, $4:node),
                                        ForeachRelationship(ALL, ":knows",
                                            ForeachRelationship(ALL, ":knows",
                                                Filter($0.id == %id,
                                                    NodeScan("Person")
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