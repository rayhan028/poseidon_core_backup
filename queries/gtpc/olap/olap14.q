Project([$0:double/$1:double],
    Project([100.0 * $1:double, 1.0 + $0:double],
        Aggregate([sum($0:double), sum($1:double)],
            Project([$0.amount:double, Case($4.data:string =~ 'PR.*', $0.amount:double, 0.0)],
                Match((ol:OrderLine)-[:hasStock]->(s:Stock)<-[:hasStock]-(i:Item)))))) 