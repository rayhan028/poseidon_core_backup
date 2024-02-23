/* Query for listing suppliers and their distributed items having the lowest stock level 
   for a certain item and certain region. */
Sort([$2:string DESC, $1:string DESC, $0:uint64 DESC],
    Project([$0.id:uint64, $4.name:string, $6.name:string, $0.name:string, $4.address:string, $4.phone:string, $4.comment:string],
        Filter($0.id:uint64 = $9:uint64 && $2.quantity:int = $10:int,
            CrossJoin(
                Filter($8.name:string =~ 'EUROP.*' && $0.data:string =~ '.*b',
                    Match((i:Item)-[:hasStock]->(s:Stock)-[:hasSupplier]->(sup:Supplier)-[:isLocatedIn]->(n:Nation)-[:isPartOf]->(r:Region))),
                GroupBy([$0.id:uint64], [min($2.quantity:int)],
                    Filter($8.name:string =~ 'EUROP.*' && $0.data:string =~ '.*b',
                        Match((i2:Item)-[:hasStock]->(s2:Stock)-[:hasSupplier]->(sup2:Supplier)-[:isLocatedIn]->(n2:Nation)-[:isPartOf]->(r2:Region))
                    )
                )
            )
        )
    )
)