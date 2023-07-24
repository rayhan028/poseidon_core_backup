Create(($0)-[r:workAt { workFrom: 2000}]->($7),
    CrossJoin(
        Create(($0)-[r:studyAt { classYear: 2001}]->($5),
            CrossJoin(
                Create(($0)-[r:hasInterest]->($3),
                    CrossJoin(
                        Create(($0)-[r:isLocatedIn]->($1),
                            CrossJoin(
                                Create((p:Person { id: 290292, firstName: 'Jose', lastName: 'Rodriguez', gender: 'male', birthday: '1980-11-23', creationDate: '2011-06-20T22:41:36.349+0000', locationIP: '170.25.1.157', browserUsed: 'Chrome', language: 'Acholi', email: 'Jose@gmail.com'})),
                                Filter($0.id == 32,
                                    NodeScan('Place')
                                )
                            )
                        ),
                        Filter($0.id == 19,
                            NodeScan('Tag')
                        )
                    )
                ),
                Filter($0.id == 21,
                    NodeScan('Organisation')
                )
            )
        ),
        Filter($0.id == 3985,
            NodeScan('Organisation')
        )
    )
)
