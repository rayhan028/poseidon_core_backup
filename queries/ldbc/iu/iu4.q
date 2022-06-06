Create(($0)-[r:hasTag]-($3),
    CrossJoin(
        Create(($0)-[r:hasModerator]-($1),
            CrossJoin(
                Create((n:Forum { id: 290292, title: "Album 14 of Marcelo Souza", creationDate: "2012-03-25T18:48:39.497+00000"})),
                Filter($0.id == 65970697,
                    NodeScan('Person')
                )
            )
        ),
        Filter($0.id == 2,
            NodeScan('Tag')
        )
    )
)