/* This query reports the total amount and quantity of all shipped orderlines given by a specific time period. 
   Additionally it informs about the average amount and quantity plus the total count of all these orderlines ordered 
   by the individual orderline number. */
Sort([$0:int ASC],
	GroupBy([$0.number:int], 
			[sum($0.quantity:int), count($0.id:int), sum($0.amount:double), avg($0.quantity:int), avg($0.amount:double)],
		Filter($0.delivery_d:datetime > pb::to_datetime('2007-01-02 00:00:00.000000'),
			NodeScan('OrderLine')
		)
	)
)
