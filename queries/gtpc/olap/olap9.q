/* This query describes how much profit has been made on a selection of items for each nation and each year. 
The result list will be sorted by the name of the nation and the financial year. */
Sort([$0:string DESC, $1:int DESC],
    GroupBy([$2:string, $0:int], [sum($1:double)],
        Project([pb::year($0.entry_d:datetime), $2.amount:double, $10.name:string],
            Expand(OUT, 'Nation',
                ForeachRelationship(FROM, 'isLocatedIn',
                    Expand(OUT, 'Supplier',
                        ForeachRelationship(FROM, 'hasSupplier', $4,
                            Filter($6.data:string =~'.*BB',
                                Match((o:Order)-[:contains]->(ol:OrderLine)-[:hasStock]->(s:Stock)<-[:hasStock]-(i:Item))
                            )
                        )
                    )
                )
            )
        )
    )
)