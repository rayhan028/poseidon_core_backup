/* The query lists the number of customers grouped and sorted by the size of 
   orders they made. The result set of the relation between customers and the 
   size of their orders is sorted by the size of orders and counts how many 
   customers have dealt the same way. */
Sort([$1:int DESC, $0:int DESC],
    GroupBy([$1:int], [count($0:uint64)],
        GroupBy([$0.id:uint64],[count($2.id:uint64)],
            Filter($2.carrier_id:int > 8,
                Match((c:Customer)-[:hasPlaced]->(o:Order))
            )
        )
    )
) 
