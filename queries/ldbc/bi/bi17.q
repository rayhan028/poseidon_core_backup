Union(
    Union(
        Union(
            GroupBy([$1:node],
                    [count($0:node)],
                Filter(udf::filterRship1($18:qresult, ":hasMember"),
                    Filter(udf::rshipExists($2:node, $17:node),
                        Expand(OUT, "Person",
                            ForeachRelationship(FROM, ":hasCreator", $13,
                                Filter($15.name == %tag,
                                    Expand(OUT, "Tag",
                                        ForeachRelationship(FROM, ":hasTag",
                                            Expand(OUT, "Comment",
                                                ForeachRelationship(FROM, ":replyOf", $4,
                                                    Filter(udf::fltrRship2($11:relationship, ":hasMember"),
                                                        Filter(udf::rshipExists($10:node, $1:node),
                                                            Expand(IN, "Forum",
                                                                ForeachRelationship(TO, ":containerOf", $6,
                                                                    Filter($8.name == %tag,
                                                                        Expand(OUT, "Tag",
                                                                            ForeachRelationship(FROM, ":hasTag", $4,
                                                                                Filter(udf::msgInterval($0:datetime, $5:datetime, %hrs),
                                                                                    Project([$2.creationDate:datetime, $6:node, $8:node, $10:node, $12:node, $12.creationDate:datetime, $14:node],
                                                                                        Expand(OUT, "Post",
                                                                                            ForeachRelationship(FROM, ":replyOf", 1, 100,
                                                                                                Expand(IN, "Comment",
                                                                                                    ForeachRelationship(TO, ":hasCreator",
                                                                                                        Filter(udf::samePerson($6:node, $10:node),
                                                                                                            Expand(OUT, "Person",
                                                                                                                ForeachRelationship(FROM, ":hasMember",
                                                                                                                    Expand(IN, "Forum",
                                                                                                                        ForeachRelationship(TO, ":containerOf", $4,
                                                                                                                            Expand(OUT, "Person",
                                                                                                                                ForeachRelationship(FROM, ":hasCreator", $2,
                                                                                                                                    Expand(OUT, "Post",
                                                                                                                                        ForeachRelationship(FROM, ":replyOf", 1, 100,
                                                                                                                                            Expand(IN, "Comment",
                                                                                                                                                ForeachRelationship(TO, ":hasTag",
                                                                                                                                                    Filter($0.name == %tag,
                                                                                                                                                        NodeScan("Tag")
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
                                        )
                                    )
                                )
                            )
                        )
                    )
                )
            ),
            GroupBy([$1:node],
                    [count($0:node)],
                Filter(udf::filterRship1($17:qresult, ":hasMember"),
                    Filter(udf::rshipExists($2:node, $16:node),
                        Expand(OUT, "Person",
                            ForeachRelationship(FROM, ":hasCreator", $12,
                                Filter($13.name == %tag,
                                    Expand(OUT, "Tag",
                                        ForeachRelationship(FROM, ":hasTag",
                                            Expand(OUT, "Comment",
                                                ForeachRelationship(FROM, ":replyOf", $4,
                                                    Filter(udf::fltrRship2($10:relationship, ":hasMember"),
                                                        Filter(udf::rshipExists($9:node, $1:node),
                                                            Expand(IN, "Forum",
                                                                ForeachRelationship(TO, ":containerOf", $4,
                                                                    Filter($7.name == %tag,
                                                                        Expand(OUT, "Tag",
                                                                            ForeachRelationship(FROM, ":hasTag", $4,
                                                                                Filter(udf::msgInterval($0:datetime, $5:datetime, %hrs),
                                                                                    Project([$2.creationDate:datetime, $6:node, $8:node, $10:node, $12:node, $12.creationDate:datetime],
                                                                                        Expand(IN, "Post",
                                                                                            ForeachRelationship(TO, ":hasCreator",
                                                                                                Filter(udf::samePerson($6:node, $10:node),
                                                                                                    Expand(OUT, "Person",
                                                                                                        ForeachRelationship(FROM, ":hasMember",
                                                                                                            Expand(IN, "Forum",
                                                                                                                ForeachRelationship(TO, ":containerOf", $4,
                                                                                                                    Expand(OUT, "Person",
                                                                                                                        ForeachRelationship(FROM, ":hasCreator", $2,
                                                                                                                            Expand(OUT, "Post",
                                                                                                                                ForeachRelationship(FROM, ":replyOf", 1, 100,
                                                                                                                                    Expand(IN, "Comment",
                                                                                                                                        ForeachRelationship(TO, ":hasTag",
                                                                                                                                            Filter($0.name == %tag,
                                                                                                                                                NodeScan("Tag")
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
                                )
                            )
                        )
                    )
                )
            )
        ),
        GroupBy([$1:node],
                [count($0:node)],
            Filter(udf::filterRship1($18:qresult, ":hasMember"),
                Filter(udf::rshipExists($2:node, $17:node),
                    Expand(OUT, "Person",
                        ForeachRelationship(FROM, ":hasCreator", $13,
                            Filter($15.name == %tag,
                                Expand(OUT, "Tag",
                                    ForeachRelationship(FROM, ":hasTag",
                                        Expand(OUT, "Comment",
                                            ForeachRelationship(FROM, ":replyOf", $4,
                                                Filter(udf::fltrRship2($11:relationship, ":hasMember"),
                                                    Filter(udf::rshipExists($10:node, $1:node),
                                                        Expand(IN, "Forum",
                                                            ForeachRelationship(TO, ":containerOf", $4,
                                                                Filter($8.name == %tag,
                                                                    Expand(OUT, "Tag",
                                                                        ForeachRelationship(FROM, ":hasTag", $4,
                                                                            Filter(udf::msgInterval($0:datetime, $5:datetime, %hrs),
                                                                                Project([$2.creationDate:datetime, $4:node, $6:node, $8:node, $10:node, $10.creationDate:datetime, $12:node],
                                                                                    Expand(OUT, "Post",
                                                                                        ForeachRelationship(FROM, ":replyOf", 1, 100,
                                                                                            Expand(IN, "Comment",
                                                                                                ForeachRelationship(TO, ":hasCreator",
                                                                                                    Filter(udf::samePerson($4:node, $8:node),
                                                                                                        Expand(OUT, "Person",
                                                                                                            ForeachRelationship(FROM, ":hasMember",
                                                                                                                Expand(IN, "Forum",
                                                                                                                    ForeachRelationship(TO, ":containerOf", $2,
                                                                                                                        Expand(OUT, "Person",
                                                                                                                            ForeachRelationship(FROM, ":hasCreator",
                                                                                                                                Expand(IN, "Post",
                                                                                                                                    ForeachRelationship(TO, ":hasTag",
                                                                                                                                        Filter($0.name == %tag,
                                                                                                                                            NodeScan("Tag")
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
                            )
                        )
                    )
                )
            )
        )
    ),
    GroupBy([$1:node],
            [count($0:node)],
        Filter(udf::filterRship1($17:qresult, ":hasMember"),
            Filter(udf::rshipExists($2:node, $16:node),
                Expand(OUT, "Person",
                    ForeachRelationship(FROM, ":hasCreator", $9,
                        Filter($13.name == %tag,
                            Expand(OUT, "Tag",
                                ForeachRelationship(FROM, ":hasTag",
                                    Expand(OUT, "Comment",
                                        ForeachRelationship(FROM, ":replyOf", $4,
                                            Filter(udf::fltrRship2($10:relationship, ":hasMember"),
                                                Filter(udf::rshipExists($9:node, $1:node),
                                                    Expand(IN, "Forum",
                                                        ForeachRelationship(TO, ":containerOf", $4,
                                                            Filter($7.name == %tag,
                                                                Expand(OUT, "Tag",
                                                                    ForeachRelationship(FROM, ":hasTag", $4,
                                                                        Filter(udf::msgInterval($0:datetime, $5:datetime, %hrs),
                                                                            Project([$2.creationDate:datetime, $4:node, $6:node, $8:node, $10:node, $10.creationDate:datetime],
                                                                                Expand(IN, "Post",
                                                                                    ForeachRelationship(TO, ":hasCreator",
                                                                                        Filter(udf::samePerson($4:node, $8:node),
                                                                                            Expand(OUT, "Person",
                                                                                                ForeachRelationship(FROM, ":hasMember",
                                                                                                    Expand(IN, "Forum",
                                                                                                        ForeachRelationship(TO, ":containerOf", $2,
                                                                                                            Expand(OUT, "Person",
                                                                                                                ForeachRelationship(FROM, ":hasCreator",
                                                                                                                    Expand(IN, "Post",
                                                                                                                        ForeachRelationship(TO, ":hasTag",
                                                                                                                            Filter($0.name == %tag,
                                                                                                                                NodeScan("Tag")
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
                )
            )
        )
    )
)