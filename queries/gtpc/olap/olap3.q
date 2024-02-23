/* Unshipped orders with the highest price amount for a customer will be listed within a given state and with orders 
   newer than a specific timestamp. This list will be sorted by the descending amount. */
Sort([$2:double ASC],
    Project([$2.id:uint64, $2.entry_d:datetime, $4.amount:double],
        Filter($0.state:string =~ 'A.*' && $2.entry_d:datetime > pb::to_datetime('2007-01-02 00:00:00.000000'),
            Match((c:Customer)-[:hasPlaced]->(o:Order)-[:contains]->(ol:OrderLine))
        )
    )
)
