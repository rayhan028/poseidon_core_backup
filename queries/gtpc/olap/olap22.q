/* This query lists how many customers within a specific range of country codes have not bought anything 
   for the whole period of time and who have a greater than average balance on their account. The county code 
   is represented by the first two characters of the phone number. */
NotExists(
    Filter($1:double > $3:double,
        CrossJoin(
            Filter($0:string == '1' || $0:string == '2' || $0:string == '3' || $0:string == '4' || $0:string == '5' || $0:string == '6' || $0:string == '7',
                Project([pb::substr($0.phone:string, 0, 1), $0.balance:double, $0:node],
                    NodeScan('Customer'))),
            Aggregate([avg($1:double)],
                Filter(($0:string == '1' || $0:string == '2' || $0:string == '3' || $0:string == '4' || $0:string == '5' || $0:string == '6' || $0:string == '7') && $1:double > 0,
                    Project([pb::substr($0.phone:string, 0, 1), $0.balance:double],
                        NodeScan('Customer'))))
        )
    ),
    Expand(OUT, 'Order', 
        ForeachRelationship(FROM, 'hasPlaced', $2))
)
