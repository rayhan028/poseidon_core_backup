/* This query finds out how many suppliers are able to supply items with given attributes sorted 
   in descending order of them. The result is grouped by the identifier of the item. */
Project([$0:string, $1:double, pb::substr($2:string, 0, 3), $3:int],
    GroupBy([$0.name:string, $0.price:double, $0.data:string], [count($4.id:uint64)],
        Except([$4.id:uint64], [$0.id:uint64],
            Filter(!($0.data:string =~ 'zz.*'),
                Match((i:Item)-[:hasStock]->(s:Stock)-[:hasSupplier]->(supp2:Supplier))),
            Distinct(
                Project([$0.id:uint64],
                    Filter($0.comment:string =~ '.*bad.*',
                        NodeScan('Supplier')))
            )
        )
    )
)
