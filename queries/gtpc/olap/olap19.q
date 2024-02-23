/* The query is for reporting the revenue achieved by some specific attributes, as the price, 
   the detailed information of the item and the quantity of the ordered amount of them. */
Aggregate([sum($0.amount:double)],
    Filter(($9.data:string =~  '.*a' && ($4.id:uint64 == 1 || $4.id:uint64 == 2 || $4.id:uint64 == 3)) ||
        ($9.data:string =~  '.*b' && ($4.id:uint64 == 1 || $4.id:uint64 == 2 || $4.id:uint64 == 4)) ||
        ($9.data:string =~  '.*c' && ($4.id:uint64 == 1 || $4.id:uint64 == 5 || $4.id:uint64 == 3)),
        HashJoin([$0.id:uint64], [$0.id:uint64],
            Filter($0.quantity:int >= 1 && $0.quantity:int <= 10,
                Match((ol:OrderLine)-[:hasStock]->(s2:Stock)<-[:hasStock]-(w:Warehouse))),
            Filter($0.quantity:int >= 1 && $0.quantity:int <= 10 && $4.price:double >= 1 && $4.price:double <= 400000, 
                Match((ol:OrderLine)-[:hasStock]->(s:Stock)<-[:hasStock]-(i:Item)))
        )
    )
)
