GroupBy([$0.ol_cnt:int], [count($0.id:int)],
    Filter($0.entry_d:datetime  >= pb::to_datetime('2007-01-02 00:00:00.000000') && $0.entry_d:datetime < pb::to_datetime('2012-01-02 00:00:00.000000') && $2.delivery_d >= $0.entry_d,
        Match((o:Order)-[:contains]->(ol:OrderLine))))
