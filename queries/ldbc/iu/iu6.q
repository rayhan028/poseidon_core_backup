Create(($0)-[r:hasTag]->($7),
    CrossJoin(
        Create(($0)-[r:isLocatedIn]->($5),
            CrossJoin(
                Create(($0)-[r:containerOf]->($1),
                    CrossJoin(
                        Filter($0.id == 37,
                            NodeScan('Forum')
                        ),
                        Create(($0)-[r:hasCreator]->($1),
                            CrossJoin(
                                Create((p:Post { id: 1234 })),
                                Filter($0.id == 1121,
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
