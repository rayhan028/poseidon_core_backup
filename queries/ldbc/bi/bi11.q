Count(
    GroupBy([$19:string],
        Append(udf::distinctBag($0:node, $1:node, $2:node),
            Filter(udf::samePerson($4:node, $18:node),
                Filter($17.creationDate > %date,
                    ForeachRelationship(ALL, ":knows", $12,
                        Filter($16.name == %country,
                            Expand(OUT, "Place",
                                ForeachRelationship(FROM, ":isPartOf",
                                    Expand(OUT, "Place",
                                        ForeachRelationship(FROM, ":isLocatedIn",
                                            Filter($11.creationDate > %date,
                                                ForeachRelationship(ALL, ":knows", $6,
                                                    Filter($10.name == %country,
                                                        Expand(OUT, "Place",
                                                            ForeachRelationship(FROM, ":isPartOf",
                                                                Expand(OUT, "Place",
                                                                    ForeachRelationship(FROM, ":isLocatedIn",
                                                                        Filter($5.creationDate > %date,
                                                                            ForeachRelationship(ALL, ":knows",
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