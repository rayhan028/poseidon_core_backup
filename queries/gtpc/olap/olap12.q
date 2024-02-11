Sort([$0:int ASC],
GroupBy([$0:int], [sum($1:int), sum($2:int)],
Project([$0.ol_cnt:int, Case($0.carrier_id:int == 1 || $0.carrier_id:int == 2, 1, 0), Case($0.carrier_id:int != 1 && $0.carrier_id:int != 2, 1, 0)],
Filter($0.entry_d:datetime <= $2.delivery_d:datetime && $2.delivery_d:datetime < pb::to_datetime('2020-01-01 00:00:00.000000'),
    Match((o:Order)-[:contains]->(ol:OrderLine)))))) 
