Limit(100, 
    Sort([$2:uint64 DESC, $1:string ASC, $0:uint64 ASC],
        Project([$0.id:uint64, $1.name:string, $2],
            GroupBy([$0, $3],
                    [count($0)],
                Expand(OUT, "Tag",
                    ForeachRelationship(FROM, ":hasTag",
                        Project([$0, $2],
                            Filter($6.name == %tagClass,
                                Expand(OUT, "TagClass",
                                    ForeachRelationship(FROM, ":hasType",
                                        Expand(OUT, "Tag",
                                            ForeachRelationship(FROM, ":hasTag",
                                                Expand(IN, ["Post", "Comment"],
                                                    ForeachRelationship(TO, ":hasCreator",
                                                        Project([$0],
                                                            Filter($4.name == %country,
                                                                Expand(OUT, "Place",
                                                                    ForeachRelationship(FROM, ":isPartOf",
                                                                        Expand(OUT, "Place",
                                                                            ForeachRelationship(FROM, ":isLocatedIn",
                                                                                GroupBy([$2],
                                                                                    ForeachRelationship(ALL, ":knows", {%min, %max},
                                                                                        Filter($0.id == %Id,
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