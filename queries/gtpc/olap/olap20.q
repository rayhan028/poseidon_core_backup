/* Suppliers in a particular nation having selected parts that may be candidates for a promotional 
   offer if the quantity of these items is more than 50 percent of the total quantity which has 
   been ordered since a certain date. */
Sort([$0:string ASC], 
    Project([$5.name:string, $5.address:string],
        HashJoin([$0:uint64], [$4.id:uint64],
            Filter($1:int > $2:int,
                Project([$0:uint64, $1:int * 2, $2:int],
                    GroupBy([$2.id:uint64, $2.quantity:int], [sum($4.quantity:int)],
                        Filter($0.data:string =~ 'co.*' && $4.delivery_d:datetime > pb::to_datetime('2010-05-23 12:00:00'),
                            Match((i:Item)-[:hasStock]->(s:Stock)<-[:hasStock]-(ol:OrderLine)))))),
            Match((n:Nation {name: 'TAIWAN'})<-[:isLocatedIn]-(supp:Supplier)<-[:hasSupplier]-(s:Stock))
        )
    )
)