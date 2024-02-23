/* Query 18 is ranking all customers who have ordered for more than a specific amount of money. */
Sort([$6:double DESC, $3:datetime ASC],
    Filter($6:double > 100.0,
        GroupBy([$0.id:uint64, $2.id:uint64, $4.cnt:int, $2.entry_d:datetime, $0.last:int,$8.id:uint64],[sum($4.amount:double)],
            Match((c:Customer)-[:hasPlaced]->(o:Order)-[:contains]->(ol:OrderLine)-[:hasStock]->(s:Stock)<-[:hasStock]-(w:Warehouse))
        )
    )
) 