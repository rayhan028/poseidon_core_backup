Union(
    Project([$4.id:uint64, $4.title:string, $6.id:uint64, $6.firstName:string, $6.lastName:string], 
        Expand(OUT, 'Person',
            ForeachRelationship(FROM, ':hasModerator',
                Expand(IN, 'Forum',
                    ForeachRelationship(TO, ':containerOf',
                        Expand(OUT, 'Post',
                            ForeachRelationship(FROM, ':replyOf', 1, 100,
                                IndexScan('Comment', 'id', 561)
                            )
                        )
                    )
                )
            )
        )
    ),
    Project([$2.id:uint64, $2.title:string, $4.id:uint64, $4.firstName:string, $4.lastName:string], 
        Expand(OUT, 'Person',
            ForeachRelationship(FROM, ':hasModerator',
                Expand(IN, 'Forum',
                    ForeachRelationship(TO, ':containerOf',
                        IndexScan('Post', 'id', 233)
                    )
                )
            )
        )
    )
)