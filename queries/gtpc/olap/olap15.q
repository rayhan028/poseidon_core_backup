Project([$3.id:uint64, $3.name:string, $3.address:string, $3.phone:uint64, $1:double],
    HashJoin([$0:uint64, $0.id:uint64],
        Filter($1:double = $2:double,
            GroupBy([$0:uint64, $1:double],[max($1:double)],
                GroupBy([$4.id:uint64],[sum($0.amount:double)],
                    Filter($0.delivery_d:datetime <= pb::to_datetime('2011-01-02 00:00:00.000000'), 
                        Match((ol:OrderLine)-[:hasStock]->(s:Stock)-[:hasSupplier]->(sup:Supplier)))))), 
        NodeScan('Supplier')
    )
)
