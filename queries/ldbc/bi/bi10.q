Limit(100, 
    Sort([$2:DESC, $1:ASC, $0:ASC],
        Project([$0.id:uint64, $1.name:string, $2],
            Aggregate([$0:COUNT],
                Group([$0, $3],
                    Expand(OUT, "Tag",
                        ForeachRelationship(FROM, ":hasTag",
                            Project([$0, $2],
                                Filter($6.name == %country2,
                                    Expand(OUT, "Tagclass",
                                        ForeachRelationship(FROM, ":hasType",
                                            Expand(OUT, "Tag",
                                                ForeachRelationship(FROM, ":hasTag",
                                                    Expand(IN, ["Post", "Comment"],
                                                        ForeachRelationship(TO, ":hasCreator",
                                                            Project([$2],
                                                                Filter($6.name == %country1,
                                                                    Expand(OUT, "Place",
                                                                        ForeachRelationship(FROM, ":isPartOf",
                                                                            Expand(OUT, "Place",
                                                                                ForeachRelationship(FROM, ":isLocatedIn",
                                                                                    Expand(OUT, "Person",
                                                                                        ForeachRelationship(FROM, [1, 100], ":knows",
                                                                                            Filter($0.id == %pid,
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
)