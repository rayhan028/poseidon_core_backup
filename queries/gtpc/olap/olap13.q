Sort([$1:int DESC, $0:int DESC],
GroupBy([$1:int], [count($0:uint64)],
    GroupBy([$0.id:uint64],[count($2.id:uint64)],
        Filter($2.carrier_id:int > 8,
            Match((c:Customer)-[:hasPlaced]->(o:Order)))))) 
