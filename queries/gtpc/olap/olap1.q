Sort([$0:int ASC],
GroupBy([$0.number:int], [sum($0.quantity:int), count($0.id:int), sum($0.amount:int), avg($0.quantity:int), avg($0.amount:int)],
	Filter($0.delivery_d:datetime > pb::to_datetime('2007-01-02 00:00:00.000000'),
		NodeScan('OrderLine'))))
