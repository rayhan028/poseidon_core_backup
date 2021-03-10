Limit(20,
    Sort([$0:DESC, $0:ASC],
        Project([$0.id:uint64, $1],
            Aggregate([$0:COUNT],
                Group([$4],
                    Filter($6.name == %tag,
                        Expand(OUT, "Tag",
                            ForeachRelationship(FROM, ":hasInterest", $4,
                                Filter($5.Lambda2,
                                    Filter(Lambda1,
                                        Expand(OUT, "Person",
                                            ForeachRelationship(FROM, ":knows",
                                                Expand(OUT, "Person",
                                                    ForeachRelationship(FROM, ":knows",
                                                        Filter($0.id == %id1,
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