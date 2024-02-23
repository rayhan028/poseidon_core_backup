/* Query for showing the bi-directional trade volume between two given nations sorted by their names 
   and the considered years. */
Sort([$1:string ASC, $0:string ASC, $2:int ASC],
    GroupBy([$0:string, $1:string, $2:int], [sum($3:double)],
        Project([pb::substr($0.state:string, 0, 1), $12.name:string, pb::year($2.entry_d:datetime), $4.amount:double],
            Filter(($10.name:string == 'CAMBODIA' && $12.name:string == 'GERMANY') || 
                    ($10.name:string == 'GERMANY' && $12.name:string == 'CAMBODIA'),
                Expand(OUT, 'Nation',
                    ForeachRelationship(FROM, 'isLocatedIn', $8,
                        Expand(OUT, 'Nation',
                            ForeachRelationship(FROM, 'isLocatedIn', $0,
                                Filter($4.delivery_d:datetime >= pb::to_datetime('2007-01-02 00:00:00.000000') && 
                                        $4.delivery_d:datetime < pb::to_datetime('2012-01-02 00:00:00.000000'),
                                    Match((c:Customer)-[:hasPlaced]->(o:Order)-[:contains]->(ol:OrderLine)-[:hasStock]->(s:Stock)-[:hasSupplier]->(sup:Supplier))
                                ) 
                            )
                        )
                    )
                )
            )
        )
    )
)