Sort([$5:double ASC],
    GroupBy([$0.id:uint64, $0.last:string, $0.city:string, $0.phone:uint64, $6.name:string], [sum($4.amount:double)],
        Expand(OUT, 'Nation',
            ForeachRelationship(FROM, 'isLocatedIn', $0,
                Filter($2.entry_d:datetime >= pb::to_datetime('2007-01-02 00:00:00.000000') && $2.entry_d:datetime <= $4.delivery_d:datetime,
                    Match((c:Customer)-[:hasPlaced]->(o:Order)-[:contains]->(ol:OrderLine))))))) 