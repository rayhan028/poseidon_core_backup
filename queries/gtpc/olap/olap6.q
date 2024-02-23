/* Query lists the total amount of archived revenue from orderlines which 
   were delivered in a specific period and a certain quantity. */
Aggregate([sum($0.amount:double)],
    Filter($0.delivery_d:datetime >= pb::to_datetime('1970-01-01 00:00:00.000') 
            && $0.delivery_d:datetime < pb::to_datetime('2020-01-01 00:00:00.000') && 
            $0.quantity:int >= 1 && $0.quantity:int <= 100000,
        NodeScan('OrderLine')
    )
)
