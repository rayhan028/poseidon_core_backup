/* The query determines the suppliers which have shipped some required items of an order not in a timely 
   manner for a given nation. */
GroupBy([$6.name:string], [count($0.id:uint64)],
    NotExists(
        Filter($2.delivery_d:datetime > $0.entry_d:datetime && $8.name:string == 'GERMANY',
            Match((o:Order)-[:contains]->(ol1:OrderLine)-[:hasStock]->(s:Stock)-[:hasSupplier]->(sup:Supplier)-[:isLocatedIn]->(n:Nation))),
        Filter($10.delivery_d:datetime > $2.delivery_d:datetime,
            Expand(OUT, 'OrderLine',
                ForeachRelationship(FROM, 'contains', $0))
        )
    )
)