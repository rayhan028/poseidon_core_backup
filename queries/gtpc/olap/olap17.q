/* The query determines the yearly loss in revenue if orders just with a quantity of more than the average 
   quantity of all orders in the system would be taken and shipped to customers. */
Project([$0:int / 2.0],
    Aggregate([sum($2.amount:double)],
        Filter($1:double >= $2.quantity:double,
            HashJoin([$0:uint64], [$0.id:uint64],
                GroupBy([$0.id:uint64],[avg($4.quantity:double)],
                    Filter($0.data:string =~ '.*b',
                        Match((i:Item)-[:hasStock]->(s:Stock)<-[:hasStock]-(ol:OrderLine)))),
                NodeScan('OrderLine')
            )
        )
    )
)
