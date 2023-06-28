Project([$2.id:uint64, $2.firstName:string, $2.lastName:string], 
    Expand(OUT, 'Person',
        ForeachRelationship(FROM, ':hasCreator',
            Filter($0.id == 12362343,
                NodeScan(['Post', 'Comment'])
            )
        )
    )
)
