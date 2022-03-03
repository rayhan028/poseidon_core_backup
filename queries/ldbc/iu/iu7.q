Create(($0)-[r:hasTag]-($7),
    CrossJoin(
        Create(($0)-[r:isLocatedIn]-($5),
            CrossJoin(
                Create(($0)-[r:replyOf]-($1),
                    CrossJoin(
                        Filter($0.id == 13743894,
                            NodeScan('Post')
                        ),
                        Create(($0)-[r:hasCreator]-($1),
                            CrossJoin(
                                Create((c:Comment { id: 12362345, creationDate: "2010-11-01T03:44:38.424+0000", locationIP: "14.99.62.99", browserUsed: "Firefox", content: "I see", length: 5 })),
                                Filter($0.id == 838375,
                                    NodeScan('Person')
                                )
                            )
                        )
                    )
                ),
                Filter($0.id == 129,
                    NodeScan('Place')
                )
            )
        ),
        Filter($0.id == 2,
            NodeScan('Tag')
        )
    )
)