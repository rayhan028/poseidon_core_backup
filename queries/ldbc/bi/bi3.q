Limit(20,
    Sort([$4:DESC, $0:ASC],
        Project([$0.id:uint64, $0.title:string, $0.creationDate:datetime, $1.id:uint64, $2],
            Aggregate([$0:COUNT],
                Group([$6, $4],
                    Filter($12.name == %tagclass,
                        Expand(OUT, "TagClass",
                            ForeachRelationship(FROM, ":hasType",
                                Expand(OUT, "Tag",
                                    ForeachRelationship(FROM, ":hasTag",
                                        Expand(OUT, ["Post", "Comment"],
                                            ForeachRelationship(FROM, ":containerOf",
                                                Expand(IN, "Forum",
                                                    ForeachRelationship(TO, ":hasModerator",
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