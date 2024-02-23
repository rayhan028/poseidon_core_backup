/* Query result for getting information about achieved revenues of nations within a given region. 
All nations are sorted by the total amount of revenue gained since the given date. */
GroupBy([$8.name:string], [sum($6.amount:double)],
    Filter($8.name:string = 'EUROPE',
        Expand(OUT, 'Region',
            ForeachRelationship(FROM, 'isPartOf', $0,
                Filter($4.entry_d:datetime >= pb::to_datetime('2007-01-02 00:00:00.000000'),
                    Match((n:Nation)<-[:isLocatedIn]-(c:Customer)-[:hasPlaced]->(o:Order)-[:contains]->(ol:OrderLine)))))))