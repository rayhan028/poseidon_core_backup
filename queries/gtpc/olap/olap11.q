/* Most important items (items which are often involved in orders and therefore often bought by customers) 
   supplied by supplier of a given nation. */
Sort([$2:int ASC],
    Project([$0:uint64, $1:int],
        Filter($1:int > $2:double,
            CrossJoin(
                GroupBy([$0.id:uint64],[sum($2.order_count:int)],
                    Filter($6.name:string = 'GERMANY',
                        Match((i:Item)-[:hasStock]->(s:Stock)-[:hasSupplier]->(sup:Supplier)-[:isLocatedIn]->(n:Nation)))),
                Project([$0:int * 0.005],
                    Aggregate([sum($0.order_cnt:int)],
                        Filter($4.name:string = 'GERMANY',
                            Match((s2:Stock)-[:hasSupplier]->(sup2:Supplier)-[:isLocatedIn]->(n2:Nation))
                        )
                    )
                )
            ) 
        )
    )
)